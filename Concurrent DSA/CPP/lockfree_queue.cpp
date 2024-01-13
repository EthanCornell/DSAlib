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
#include <thread>
#include <iostream>

template<typename T>
class LockFreeQueue {
    struct Node {
        std::shared_ptr<T> data;
        std::atomic<std::shared_ptr<Node>> next;

        Node(T value) : data(std::make_shared<T>(value)), next(nullptr) {}
    };

    std::atomic<std::shared_ptr<Node>> head;
    std::atomic<std::shared_ptr<Node>> tail;

public:
    LockFreeQueue() : head(std::make_shared<Node>(T())), tail(head.load()) {}

    void enqueue(T value) {
        auto newNode = std::make_shared<Node>(value);
        while(true) {
            auto tailNode = tail.load();
            auto next = tailNode->next.load();
            if (tailNode == tail.load()) {
                if (!next) {
                    if (tailNode->next.compare_exchange_weak(next, newNode)) {
                        tail.compare_exchange_weak(tailNode, newNode);
                        return;
                    }
                } else {
                    tail.compare_exchange_weak(tailNode, next);
                }
            }
        }
    }

    bool dequeue(T& value) {
        while(true) {
            auto headNode = head.load();
            auto tailNode = tail.load();
            auto next = headNode->next.load();
            if (headNode == head.load()) {
                if (headNode == tailNode) {
                    if (!next) {
                        return false;
                    }
                    tail.compare_exchange_weak(tailNode, next);
                } else {
                    value = next->data;
                    if (head.compare_exchange_weak(headNode, next)) {
                        return true;
                    }
                }
            }
        }
    }
};


// To test the lock-free queue for the ABA problem, we would need to create a scenario where a node is enqueued, dequeued, and then a new node (possibly with the same value) is enqueued again, 
// while multiple threads are involved. We can set up a basic test scenario with multiple threads:

// Thread 1: Enqueues and dequeues elements repeatedly.
// Thread 2: Enqueues elements with the same values.
int main() {
    LockFreeQueue<int> queue;

    // Thread 1: Enqueue and Dequeue
    std::thread t1([&]() {
        for(int i = 0; i < 1000; ++i) {
            queue.enqueue(i);
            int value;
            if (queue.dequeue(value)) {
                // Handle dequeued value
                std::cout << "Dequeued: " << value << std::endl;
            }
        }
    });

    // Thread 2: Only Enqueue
    std::thread t2([&]() {
        for(int i = 0; i < 1000; ++i) {
            queue.enqueue(i);
        }
    });

    t1.join();
    t2.join();

    return 0;
}
