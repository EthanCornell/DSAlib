

#include <iostream>
#include <memory>
#include <mutex>
#include <condition_variable>

template <typename T>
class ConditionVariableLinkedList
{
private:
    struct Node
    {
        T data;
        std::shared_ptr<Node> next;
        Node(T data) : data(data), next(nullptr) {}
    };

    std::shared_ptr<Node> head;
    std::mutex mutex;
    std::condition_variable cv;

public:
    ConditionVariableLinkedList() : head(nullptr) {}

    void insert(T data) {
        std::lock_guard<std::mutex> lock(mutex);
        auto newNode = std::make_shared<Node>(data);
        if (!head) {
            head = newNode;
        } else {
            newNode->next = head;
            head = newNode;
        }
        cv.notify_one();
    }

    bool remove(T data) {
        std::unique_lock<std::mutex> lock(mutex);
        while (!head) { // Wait until the list is not empty
            cv.wait(lock);
        }
        auto temp = head, prev = head;
        while (temp != nullptr) {
            if (temp->data == data) {
                if (temp == head) {
                    head = head->next;
                } else {
                    prev->next = temp->next;
                }
                return true;
            }
            prev = temp;
            temp = temp->next;
        }
        return false;
    }

    //another solution
    // void insert(T data)
    // {
    //     std::lock_guard<std::mutex> lock(mutex);
    //     auto newNode = std::make_shared<Node>(data);
    //     newNode->next = head; // This is safe even if head is nullptr
    //     head = newNode;
    //     cv.notify_one(); // Notify potentially waiting remove operations
    // }

    // bool remove(T data)
    // {
    //     std::unique_lock<std::mutex> lock(mutex);
    //     if (!head)
    //     { // Immediately return if the list is empty
    //         return false;
    //     }
    //     auto temp = head;
    //     std::shared_ptr<Node> prev = nullptr;
    //     while (temp != nullptr)
    //     {
    //         if (temp->data == data)
    //         {
    //             if (prev)
    //             { // Not the first node
    //                 prev->next = temp->next;
    //             }
    //             else
    //             { // First node
    //                 head = temp->next;
    //             }
    //             return true; // Data found and removed
    //         }
    //         prev = temp;
    //         temp = temp->next;
    //     }
    //     return false; // Data not found
    // }

    bool search(T data)
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto temp = head;
        while (temp != nullptr)
        {
            if (temp->data == data)
            {
                return true;
            }
            temp = temp->next;
        }
        return false;
    }
};

// g++ -pg -std=c++17 -o CVLL ConditionVariableLinkedList.cpp -lpthread -O3

#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
// #include "ConditionVariableLinkedList.hpp" // Adjust the include path as needed

void correctnessTest()
{
    ConditionVariableLinkedList<int> list;

    // Insert elements
    list.insert(1);
    list.insert(2);
    list.insert(3);

    // Search elements
    assert(list.search(1) == true);
    assert(list.search(2) == true);
    assert(list.search(3) == true);
    assert(list.search(4) == false); // Should not be found
    // Prepare a variable to hold the removed value
    int removedValue;

    // Remove elements
    // Instead of passing literals directly, use `removedValue` as an argument
    assert(list.remove(2) == true);
    assert(list.search(2) == false); // 2 should not be found after removal
    assert(list.remove(4) == false); // Since 4 was never added, it should not be found for removal.

    std::cout << "Correctness test passed." << std::endl;
}

void threadSafeTest()
{
    ConditionVariableLinkedList<int> list;
    const int numThreads = 8; // Using an even number to balance inserters and removers
    const int numOperations = 100;

    auto worker = [&list](int threadId)
    {
        if (threadId % 2 == 0)
        {
            for (int i = 0; i < numOperations; ++i)
            {
                list.insert(i);
            }
        }
        else
        {
            int removedData;
            for (int i = 0; i < numOperations; ++i)
            {
                if (list.remove(removedData))
                {
                    std::cout << "Thread " << threadId << " removed: " << removedData << std::endl;
                }
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(worker, i);
    }

    for (auto &t : threads)
    {
        t.join();
    }

    std::cout << "Thread safety test completed." << std::endl;
}

void performanceTest()
{
    for (int numThreads = 1; numThreads <= 128; numThreads++)
    {
        ConditionVariableLinkedList<int> list;
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i)
        {
            threads.emplace_back([&list]()
                                 {
                for (int j = 0; j < 1000; ++j) { // Each thread performs 1000 operations
                    list.insert(j);
                    list.search(j);
                    list.remove(j);
                } });
        }

        for (auto &t : threads)
        {
            t.join();
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        double opsPerMs = (numThreads * 3000) / elapsed.count(); // 3000 operations: insert, search, remove

        std::cout << "Threads: " << numThreads << ", Ops/ms: " << opsPerMs << ", Time: " << elapsed.count() << " ms" << std::endl;
    }
}

int main()
{
    correctnessTest(); // Validate basic functionality
    threadSafeTest();  // Ensure thread safety
    performanceTest(); // Measure performance under concurrent access

    return 0;
}
