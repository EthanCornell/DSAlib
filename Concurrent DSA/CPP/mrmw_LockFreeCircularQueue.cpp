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
#include <vector>
#include <memory>
#include <iostream>
#include <thread>
#include <cassert>


template<typename T>
class LockFreeCircularQueue {
public:
    explicit LockFreeCircularQueue(size_t capacity)
        : capacity_(capacity), head_(0), tail_(0), data_(capacity) {
        assert(capacity > 0); // Ensure the capacity is greater than 0.
        std::cout << "Initializing LockFreeCircularQueue with capacity: " << capacity << std::endl;
    }

    bool enqueue(T item) {
        size_t currentTail = tail_.load(std::memory_order_relaxed);
        size_t nextTail = nextIndex(currentTail);
        std::cout << "currentTail = " << currentTail << ", nextTail = " << nextTail << std::endl;
        if(is_full()) { // Check if the queue is full before attempting to enqueue.
            std::cout << "Error: Enqueue failed because the queue is full." << std::endl;
            return false;
        }
        data_[currentTail] = std::move(item);
        tail_.store(nextTail, std::memory_order_release);
        std::cout << "Enqueued item at position: " << currentTail << ", with item = " << item << ". New tail position: " << nextTail << std::endl;
        return true;
    }

    bool dequeue(T& item) {
        if(is_empty()) { // Check if the queue is empty before attempting to dequeue.
            std::cout << "Error: Dequeue failed because the queue is empty." << std::endl;
            return false;
        }
        size_t currentHead = head_.load(std::memory_order_relaxed);
        item = std::move(data_[currentHead]);
        head_.store(nextIndex(currentHead), std::memory_order_release);
        std::cout << "Dequeued item from position: " << currentHead << ". New head position: " << nextIndex(currentHead) << std::endl;
        return true;
    }
    
    bool is_empty() const {
        bool empty = head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
        if(empty) {
            std::cout << "Queue is empty." << std::endl;
        }
        return empty;
    }

    bool is_full() const {
        // Compute the next tail and current head to determine if the queue is full.
        size_t nextTail = nextIndex(tail_.load(std::memory_order_relaxed));
        size_t currentHead = head_.load(std::memory_order_acquire);

        // The queue is full if incrementing the tail would make it equal to the head.
        bool full = nextTail == currentHead;
        if(full) {
            std::cout << "Queue is full." << std::endl;
        } else {
            std::cout << "Queue is not full." << std::endl;
        }
        return full;
    }

private:
    size_t nextIndex(size_t index) const {
        return (index + 1) % capacity_;
    }

    const size_t capacity_;
    std::atomic<size_t> head_, tail_;
    std::vector<T> data_;
};


// Example usage and test function.
void testQueue() {
    LockFreeCircularQueue<int> queue(1024);

    // Producer thread.
    std::thread producer([&]{
        for(int i = 0; i < 1000; ++i) {
            while(!queue.enqueue(i)) {
                // Retry until enqueue is successful.
            }
        }
    });

    // Consumer thread.
    std::thread consumer([&]{
        for(int i = 0; i < 1000; ++i) {
            int item;
            while(!queue.dequeue(item)) {
                // Retry until dequeue is successful.
            }
            assert(item == i); // Validate the dequeued item.
        }
    });

    producer.join();
    consumer.join();
    std::cout << "Basic functions Test completed successfully." << std::endl;
}


void deadlockTest() {
    LockFreeCircularQueue<int> queue(10); // Small size to increase contention.

    auto worker = [&](int id) {
        for(int i = 0; i < 100; ++i) {
            if(id % 2 == 0) {
                while(!queue.enqueue(i)) {}
            } else {
                int item;
                while(!queue.dequeue(item)) {}
            }
        }
    };

    std::thread t1(worker, 1);
    std::thread t2(worker, 2);

    t1.join();
    t2.join();

    std::cout << "Deadlock test completed." << std::endl;
}


void stressTest() {
    const int numThreads = 10; // Adjust based on your testing needs.
    LockFreeCircularQueue<int> queue(1000); // Sufficiently large queue.

    auto producer = [&](int startValue) {
        for(int i = startValue; i < startValue + 1000; ++i) {
            while(!queue.enqueue(i)) {}
        }
    };

    auto consumer = [&]() {
        for(int i = 0; i < 1000; ++i) {
            int item;
            while(!queue.dequeue(item)) {}
        }
    };

    std::vector<std::thread> threads;
    for(int i = 0; i < numThreads / 2; ++i) {
        threads.push_back(std::thread(producer, i * 1000));
        threads.push_back(std::thread(consumer));
    }

    for(auto& t : threads) {
        t.join();
    }

    std::cout << "Stress test completed." << std::endl;
}

void edgeCaseTest() {
    LockFreeCircularQueue<int> queue(2); // Small size to test edge cases.

    assert(queue.enqueue(1)); // Should succeed.
    assert(!queue.enqueue(3)); // Should fail (queue full).

    int item;
    assert(queue.dequeue(item) && item == 1); // Should succeed and get 1.
    assert(!queue.dequeue(item)); // Should fail (queue empty).

    std::cout << "Edge case test completed." << std::endl;
}

void emptyQueueTest() {
    LockFreeCircularQueue<int> queue(5);
    int item;
    assert(!queue.dequeue(item) && "Dequeue should fail on an empty queue");
    std::cout << "Empty queue test passed.\n";
}

void fullQueueTest() {
    LockFreeCircularQueue<int> queue(1); // Small capacity for easy testing
    assert(!queue.enqueue(2) && "Enqueue should fail on a full queue");
    std::cout << "Full queue test passed.\n";
}

void concurrentEnqueueDequeueTest() {
    LockFreeCircularQueue<int> queue(100);
    std::atomic<int> enqueues(0), dequeues(0);

    auto producer = [&]() {
        for (int i = 0; i < 1000; ++i) {
            if (queue.enqueue(i)) enqueues++;
        }
    };

    auto consumer = [&]() {
        int item;
        for (int i = 0; i < 1000; ++i) {
            if (queue.dequeue(item)) dequeues++;
        }
    };

    std::thread producerThread(producer);
    std::thread consumerThread(consumer);

    producerThread.join();
    consumerThread.join();

    assert(enqueues == dequeues && "Mismatch between enqueues and dequeues");
    std::cout << "Concurrent enqueue and dequeue test passed.\n";
}

void mixedEnqueueDequeueTest() {
    LockFreeCircularQueue<int> queue(10);
    int item;

    // Enqueue 5 items
    for (int i = 0; i < 5; ++i) {
        assert(queue.enqueue(i) && "Enqueue should succeed");
    }

    // Dequeue 3 items
    for (int i = 0; i < 3; ++i) {
        assert(queue.dequeue(item) && "Dequeue should succeed");
    }

    // Enqueue 3 more items
    for (int i = 5; i < 8; ++i) {
        assert(queue.enqueue(i) && "Enqueue should succeed");
    }

    // Dequeue the remaining 5 items
    for (int i = 0; i < 5; ++i) {
        assert(queue.dequeue(item) && "Dequeue should succeed");
    }

    // Ensure the queue is empty
    assert(!queue.dequeue(item) && "Queue should be empty");
    std::cout << "Mixed enqueue and dequeue test passed.\n";
}

// void memoryReclamationTest() {
//     LockFreeCircularQueue<std::shared_ptr<int>> queue(10);

//     auto producer = [&]() {
//         for(int i = 0; i < 100; ++i) {
//             queue.enqueue(std::make_shared<int>(i));
//         }
//     };

//     auto consumer = [&]() {
//         std::shared_ptr<int> item;
//         for(int i = 0; i < 100; ++i) {
//             while(!queue.dequeue(item)) {}
//             // `item` goes out of scope and is destructed automatically.
//         }
//     };

//     std::thread t1(producer);
//     std::thread t2(consumer);

//     t1.join();
//     t2.join();

//     std::cout << "Memory reclamation test completed." << std::endl;
// }


void integerOverflowTest() {
    // This test is more conceptual since it requires the indices to overflow,
    // which is impractical to achieve in a test due to the high number of iterations required.
    // Ensure your nextIndex() or equivalent method correctly handles wrapping.
    std::cout << "Integer overflow handling is more conceptual and should be ensured by design." << std::endl;
}

int main() {
    // testQueue(); 
    // deadlockTest();
    // stressTest();
    
    // emptyQueueTest();
    fullQueueTest();
    edgeCaseTest();
    // concurrentEnqueueDequeueTest();
    // mixedEnqueueDequeueTest();
    // memoryReclamationTest();
    // integerOverflowTest();

    return 0;
}


// g++ -pg -o mrmw ./mrmw_LockFreeCircularQueue.cpp -pthread  -O3
