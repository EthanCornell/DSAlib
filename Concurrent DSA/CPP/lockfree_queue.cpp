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
#include <memory>
#include <thread>

template <typename T>
class LockFreeQueue {
    struct Node {
        std::shared_ptr<T> data;
        std::atomic<Node*> next;  // Changed to atomic

        Node(T value) : data(std::make_shared<T>(value)), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* dummy = new Node(T());  // Create a dummy node
        head.store(dummy);
        tail.store(dummy);
    }

    ~LockFreeQueue() {
        Node* node;
        while ((node = head.load())) {  // Corrected: Added parentheses
            head.store(node->next);
            delete node;
        }
    }

    void enqueue(T value) {
        Node* newNode = new Node(value);
        while (true) {
            Node* tailNode = tail.load();
            Node* next = tailNode->next;

            if (tailNode == tail.load()) {
                if (!next) {
                    Node* nullNode = nullptr;
                    // Use atomic compare_exchange_weak directly on the atomic member
                    if (std::atomic_compare_exchange_weak(&(tailNode->next), &nullNode, newNode)) {
                        std::atomic_compare_exchange_weak(&tail, &tailNode, newNode);
                        return;
                    }
                } else {
                    std::atomic_compare_exchange_weak(&tail, &tailNode, next);
                }
            }
        }
    }

    bool dequeue(std::shared_ptr<T>& value) {
        while (true) {
            Node* headNode = head.load();
            Node* tailNode = tail.load();
            Node* next = headNode->next;
            if (headNode == head.load()) {
                if (headNode == tailNode) {
                    if (!next) {
                        return false;
                    }
                    std::atomic_compare_exchange_weak(&tail, &tailNode, next);
                } else {
                    value = next->data;
                    if (std::atomic_compare_exchange_weak(&head, &headNode, next)) {
                        delete headNode;
                        return true;
                    }
                }
            }
        }
    }
};

// To test the lock-free queue for the ABA problem, we would need to create a
// scenario where a node is enqueued, dequeued, and then a new node (possibly
// with the same value) is enqueued again, while multiple threads are involved.
// We can set up a basic test scenario with multiple threads:

// Thread 1: Enqueues and dequeues elements repeatedly.
// Thread 2: Enqueues elements with the same values.
int main() {
    LockFreeQueue<int> queue;

    // Thread 1: Enqueue and Dequeue
    std::thread t1([&]() {
        for (int i = 0; i < 1000; ++i) {
            queue.enqueue(i);
            std::shared_ptr<int> value;  // Changed from int to std::shared_ptr<int>
            if (queue.dequeue(value)) {
                // Handle dequeued value
                std::cout << "Thread 1: Dequeued: " << *value << std::endl;  // Dereference the pointer to access the value
            }
        }
    });

    // Thread 2: Only Enqueue
    std::thread t2([&]() {
        for (int i = 0; i < 1000; ++i) {
            queue.enqueue(i);
            std::cout << "Thread 2 : Equeued: " << i << std::endl;  // Dereference the pointer to access the value
        }
    });

    t1.join();
    t2.join();

    return 0;
}
