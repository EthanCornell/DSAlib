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
// NaiveLinkedList
#include <mutex>
#include <iostream>

template<typename T>
class NaiveLinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(T data) : data(data), next(nullptr) {}
    };

    Node* head;
    std::mutex mtx;

public:
    NaiveLinkedList() : head(nullptr) {}

    ~NaiveLinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    void insert(T data) {
        std::lock_guard<std::mutex> lock(mtx);
        Node* newNode = new Node(data);
        newNode->next = head;
        head = newNode;
    }

    bool search(T data) {
        std::lock_guard<std::mutex> lock(mtx);
        Node* current = head;
        while (current != nullptr) {
            if (current->data == data) return true;
            current = current->next;
        }
        return false;
    }

    bool remove(T data) {
        std::lock_guard<std::mutex> lock(mtx);
        Node *current = head, *prev = nullptr;
        while (current != nullptr) {
            if (current->data == data) {
                if (prev == nullptr) {
                    head = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }
};

// g++ -pg -std=c++17 -o NLL NaiveLinkedList.cpp -lpthread -O3
// valgrind --leak-check=full --show-leak-kinds=all ./NLL


#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
// #include "NaiveLinkedList.hpp" // Include the NaiveLinkedList header if it's in a separate file

void correctnessTest() {
    NaiveLinkedList<int> list;

    // Insert elements
    list.insert(1);
    list.insert(2);
    list.insert(3);

    // Search elements
    assert(list.search(1) == true);
    assert(list.search(2) == true);
    assert(list.search(3) == true);
    assert(list.search(4) == false); // Should not be found

    // Remove elements
    assert(list.remove(2) == true);
    assert(list.search(2) == false); // Should not be found after removal
    assert(list.remove(4) == false); // Should not be found for removal

    std::cout << "Correctness test passed." << std::endl;
}

void threadSafeTest() {
    NaiveLinkedList<int> list;
    const int numThreads = 8;      // Example: Adjust based on your testing needs
    const int numOperations = 100; // Operations per thread

    auto worker = [&list](int threadId) {
        for (int i = 0; i < numOperations; ++i) {
            // Even threads insert and search, odd threads search and remove
            if (threadId % 2 == 0) {
                list.insert(i);
                list.search(i);
            } else {
                list.search(i);
                list.remove(i);
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Thread safety test completed." << std::endl;
}

void performanceTest() {
    for (int numThreads = 1; numThreads <= 128; numThreads++) {
        NaiveLinkedList<int> list;
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back([&list]() {
                for (int j = 0; j < 1000; ++j) { // Each thread performs 1000 operations
                    list.insert(j);
                    list.search(j);
                    list.remove(j);
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        double opsPerMs = (numThreads * 3000) / elapsed.count(); // 3000 operations: insert, search, remove

        std::cout << "Threads: " << numThreads << ", Ops/ms: " << opsPerMs << ", Time: " << elapsed.count() << " ms" << std::endl;
    }
}

int main() {
    correctnessTest(); // Validate basic functionality
    threadSafeTest();  // Ensure thread safety
    performanceTest(); // Measure performance under concurrent access

    return 0;
}
