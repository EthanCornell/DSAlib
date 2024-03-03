#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include <mutex>
#include "HazardPointer.hpp"

template <typename T>
class LockFreeLinkedList
{
private:
    struct Node
    {
        T data;
        std::atomic<Node *> next;
        Node(T data) : data(data), next(nullptr) {}
    };

    std::atomic<Node *> head;
    HPManager<Node> hpManager; // Assuming HPManager is templated on the node type

public:
    LockFreeLinkedList() : head(nullptr) {}

    ~LockFreeLinkedList()
    {
        Node *current = head.load();
        while (current != nullptr)
        {
            Node *next = current->next.load();
            delete current;
            current = next;
        }
    }

    void insert(T data)
    {
        Node *newNode = new Node(data);
        Node *expectedHead = head.load(std::memory_order_relaxed);
        newNode->next = expectedHead; // Initialize newNode->next
        // Try to set the new node as the new head
        while (!head.compare_exchange_weak(expectedHead, newNode,
                                           std::memory_order_release,
                                           std::memory_order_relaxed))
        {
            // CAS failed, expectedHead was updated with the current head,
            // Update newNode->next and try again
            newNode->next = expectedHead;
        }
    }

    // Safe search using hazard pointers
    bool searchSafe(T data)
    {
        HazardPointer<Node> *hp = hpManager.acquireHazardPointer(); // Assuming HPManager is now correctly handling Node types
        Node *current = head.load(std::memory_order_acquire);
        while (current != nullptr)
        {
            hp->pointer.store(current, std::memory_order_release); // Correctly typed now
            if (current->data == data)
            {
                hpManager.releaseHazardPointer(hp);
                return true; // Data found
            }
            current = current->next.load(std::memory_order_acquire);
        }
        hpManager.releaseHazardPointer(hp);
        return false; // Data not found
    }

    // Safe remove using hazard pointers
    bool removeSafe(T data)
    {
        std::vector<Node *> retiredNodes; // For later safe deletion
        HazardPointer<Node> *hp = hpManager.acquireHazardPointer();
        Node *prev = nullptr;
        Node *current = head.load(std::memory_order_acquire);
        bool deleted = false; // Flag to indicate deletion

        while (current != nullptr)
        {
            Node *next = current->next.load(std::memory_order_acquire);
            hp->pointer.store(current, std::memory_order_release); // Protect with hazard pointer

            if (current->data == data)
            {
                // Attempt to unlink the node safely
                if (prev == nullptr)
                {
                    if (head.compare_exchange_strong(current, next, std::memory_order_release))
                    {
                        retiredNodes.push_back(current); // Schedule for safe deletion
                        deleted = true;                  // Indicate successful deletion
                        break;                           // Exit loop after deletion
                    }
                }
                else
                {
                    if (prev->next.compare_exchange_strong(current, next, std::memory_order_release))
                    {
                        retiredNodes.push_back(current); // Schedule for safe deletion
                        deleted = true;                  // Indicate successful deletion
                        break;                           // Exit loop after deletion
                    }
                }
            }
            prev = current; // Move prev forward
            current = next; // Move current forward
        }

        // Safe deletion of retired nodes, ensuring they are no longer accessed
        for (Node *node : retiredNodes)
        {
            if (!hpManager.isPointerHazardous(node))
            {
                delete node; // Now safe to delete
            }
            // If still hazardous, consider adding back to a global retire list for later reclamation
        }

        hpManager.releaseHazardPointer(hp);
        return deleted; // Return true if a node was removed
    }
};


#include <unistd.h>
#include <fstream> 
// #include <iostream>
// #include <vector>
// #include <thread>

// Include your LockFreeLinkedList and HazardPointer headers here

// Function to measure memory usage (implementation depends on your platform)
size_t getMemoryUsage()
{
    long pages = sysconf(_SC_PAGESIZE); // Get the system's page size
    std::ifstream statm("/proc/self/statm");
    size_t size;
    statm >> size; // The first field in /proc/self/statm is total program size
    statm.close();
    return size * pages; // Convert from pages to bytes
}

void threadWork(LockFreeLinkedList<int> &list)
{
    // Example workload for each thread
    for (int i = 0; i < 1000; ++i)
    {
        list.insert(rand() % 1000);
    }
}

int main()
{
    for (int numThreads = 1; numThreads <= 128; numThreads *= 2)
    {
        LockFreeLinkedList<int> list;

        auto startMemUsage = getMemoryUsage();

        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i)
        {
            threads.emplace_back(threadWork, std::ref(list));
        }

        for (auto &t : threads)
        {
            t.join();
        }

        auto endMemUsage = getMemoryUsage();

        // Convert memory usage from bytes to megabytes
        double startMemUsageMB = startMemUsage / (1024.0 * 1024.0);
        double endMemUsageMB = endMemUsage / (1024.0 * 1024.0);
        double deltaMemUsageMB = (endMemUsage - startMemUsage) / (1024.0 * 1024.0);

        std::cout << "Threads: " << numThreads
                  << ", Start Memory: " << startMemUsageMB << " MB"
                  << ", End Memory: " << endMemUsageMB << " MB"
                  << ", Total RAM consumption: " << deltaMemUsageMB << " MB" << std::endl;
    }

    return 0;
}

// g++ -pg -std=c++17 -o LFLLmemtest LockFreeLinkedlist_memtest.cpp -lpthread -O3
// valgrind --leak-check=full --show-leak-kinds=all ./LFLLmemtest
// gprof ./LFLLmemtest  gmon.out>analysis.txt