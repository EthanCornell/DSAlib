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
#include <memory>
#include <iostream>

template<typename T>
class Node {
public:
    T key;
    std::shared_ptr<Node<T>> next; // Changed to non-atomic std::shared_ptr
    std::atomic<bool> marked; // Keeps atomic for simple types

    Node(T key) : key(key), next(nullptr), marked(false) {}
};

template<typename T>
class NonBlockingLinkedList {
private:
    std::shared_ptr<Node<T>> head;
    std::shared_ptr<Node<T>> tail;

public:
    NonBlockingLinkedList() {
        head = std::make_shared<Node<T>>(T{}); // Assuming T can be default constructed
        tail = std::make_shared<Node<T>>(T{});
        std::atomic_store(&head->next, tail); // Use atomic_store for setting the tail
    }

    bool insert(T key) {
        while (true) {
            auto [left_node, right_node] = searchForNodes(key);
            if (right_node != tail && right_node->key == key)
                return false; // Key already exists
            auto new_node = std::make_shared<Node<T>>(key);
            new_node->next = right_node;
            if (std::atomic_compare_exchange_weak(&left_node->next, &right_node, new_node))
                return true; // Successfully inserted
        }
    }

    bool remove(T key) {
        while (true) {
            auto [left_node, right_node] = searchForNodes(key);
            if (right_node == tail || right_node->key != key)
                return false; // Key not found

            auto right_node_next = std::atomic_load(&right_node->next);
            if (std::atomic_compare_exchange_weak(&right_node->next, &right_node_next, right_node_next)) {
                if (std::atomic_compare_exchange_weak(&left_node->next, &right_node, right_node_next))
                    return true; // Successfully removed
                // Else, retry
            }
        }
    }

    bool search(T key) {
        auto [left_node, right_node] = searchForNodes(key);
        return !(right_node == tail || right_node->key != key);
    }

private:
    std::pair<std::shared_ptr<Node<T>>, std::shared_ptr<Node<T>>> searchForNodes(T search_key) {
        std::shared_ptr<Node<T>> left_node = head, right_node;

        do {
            std::shared_ptr<Node<T>> t = head, t_next;
            do {
                if (!is_marked(std::atomic_load(&t->next))) { // Use atomic_load to access next
                    left_node = t;
                }
                t = get_unmarked(std::atomic_load(&t->next)); // Use atomic_load here as well
                if (t == tail) break;
                t_next = std::atomic_load(&t->next);
            } while (is_marked(t_next) || (t->key < search_key && t != tail));

            right_node = t;

            // Check nodes are adjacent or remove marked nodes
            if (std::atomic_load(&left_node->next) == right_node) {
                if (right_node != tail && is_marked(std::atomic_load(&right_node->next)))
                    continue;
                else
                    break;
            }

        } while (true);

        return {left_node, right_node};
    }

    // Check if a node is marked as logically deleted
    bool is_marked(const std::shared_ptr<Node<T>>& node) {
        return node->marked.load();
    }

    // Assuming marking is done elsewhere, this function would be more about ensuring
    // the node's next pointer is correctly handled rather than modifying the node's marked state.
    std::shared_ptr<Node<T>> get_unmarked(const std::shared_ptr<Node<T>>& node) {
        // This function doesn't change the node but would be used in conjunction with is_marked
        // to ignore nodes that are marked. In a true low-level implementation, you'd strip
        // the marking bits to get the actual pointer, but here we just return the node itself.
        return node; // No change needed for std::shared_ptr, but logic should consider is_marked().
    }
};



// g++ -pg -std=c++17 -o NBLL NonBlockingLinkedList.cpp -lpthread -O3

#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <cassert>

// class NonBlockingLinkedList { // Assume this class is already defined as per previous discussion
// public:
//     // Assume insert, search, and remove functions are correctly implemented as discussed
//     bool insert(int value);
//     bool search(int value);
//     bool remove(int value);
// };

void correctnessTest() {
    NonBlockingLinkedList<int> list;

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
    assert(list.search(2) == false); // 2 should not be found after removal
    assert(list.remove(4) == false); // Since 4 was never added, it should not be found for removal.

    std::cout << "Correctness test passed." << std::endl;
}

void threadSafeTest() {
    NonBlockingLinkedList<int> list;
    const int numThreads = 8; // Using an even number to balance inserters and removers
    const int numOperations = 100;

    auto worker = [&list](int threadId) {
        if (threadId % 2 == 0) {
            for (int i = 0; i < numOperations; ++i) {
                list.insert(i);
            }
        } else {
            for (int i = 0; i < numOperations; ++i) {
                list.remove(i); // Assume the data to be removed is known
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto &t : threads) {
        t.join();
    }

    std::cout << "Thread safety test completed." << std::endl;
}

void performanceTest() {
    for (int numThreads = 1; numThreads <= 128; numThreads++) {
        NonBlockingLinkedList<int> list;
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

        for (auto &t : threads) {
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
