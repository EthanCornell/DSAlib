/*
 * Copyright (c) Cornell University.
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: I-Hsuan (Ethan) Huang
 * Email: ih246@cornell.edu
 */

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

#include <chrono>

void benchmark(size_t numThreads) {
    LockFreeLinkedList<int> list;
    const int numElements = 10000; // Number of operations per thread

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back([&list, i, numElements]() {
            for (int n = 0; n < numElements; ++n) {
                int value = i * numElements + n;
                list.insert(value);
                list.searchSafe(value);
                list.removeSafe(value);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    double ms = duration.count();
    double opsPerMs = (numThreads * numElements * 3) / ms; // Multiply by 3 because we insert, search, and remove

    std::cout << "Threads: " << numThreads << ", Time: " << ms << " ms, Ops/ms: " << opsPerMs << std::endl;
}

int main() {
    for (size_t numThreads = 1; numThreads <= 4; numThreads*=2) { // Doubling the number of threads in each step
        benchmark(numThreads);
    }
    return 0;
}

// g++ -pg -std=c++17 -o LFLL LockFreeLinkedlist.cpp -lpthread -O3
// valgrind --leak-check=full --show-leak-kinds=all ./LFLL
