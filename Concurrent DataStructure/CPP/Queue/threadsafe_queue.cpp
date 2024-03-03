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

// To implement a thread-safe queue in C++, you need to ensure that concurrent access to the queue by multiple threads does not lead to race conditions, 
// data corruption, or other concurrency issues.

//thread safe queue
#include <mutex>
#include <condition_variable>
#include <queue>

template<typename T>
class ThreadsafeQueue {
private:
    mutable std::mutex mutex_;
    std::queue<T> queue_;
    std::condition_variable cond_var_;

public:
    ThreadsafeQueue() = default;

    // Delete copy constructor and assignment operator
    ThreadsafeQueue(const ThreadsafeQueue<T>&) = delete;
    ThreadsafeQueue& operator=(const ThreadsafeQueue<T>&) = delete;

    // Move constructor
    ThreadsafeQueue(ThreadsafeQueue<T>&& other) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_ = std::move(other.queue_);
    }

    virtual ~ThreadsafeQueue() {}

    // Add element to the queue
    void enqueue(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value));
        cond_var_.notify_one();
    }

    // Try to retrieve element from the queue
    bool try_dequeue(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    // Retrieve element from the queue, block if the queue is empty
    T dequeue() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var_.wait(lock, [this]{ return !queue_.empty(); });
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }
};




#include <iostream>
#include <thread>
#include <vector>

int main() {
    ThreadsafeQueue<int> queue;

    // Create a number of producer threads
    auto producer = [&queue](int id) {
        for (int i = 0; i < 10; ++i) {
            queue.enqueue(i + id * 100);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    };

    // Create a number of consumer threads
    auto consumer = [&queue](int id) {
        for (int i = 0; i < 10; ++i) {
            int value = queue.dequeue();
            std::cout << "Consumer " << id << " dequeued: " << value << std::endl;
        }
    };

    // Start 3 producer threads
    std::vector<std::thread> producers;
    for (int i = 0; i < 3; ++i) {
        producers.emplace_back(producer, i);
    }

    // Start 3 consumer threads
    std::vector<std::thread> consumers;
    for (int i = 0; i < 3; ++i) {
        consumers.emplace_back(consumer, i);
    }

    // Join the producer threads with the main thread
    for (auto& producer_thread : producers) {
        producer_thread.join();
    }

    // Join the consumer threads with the main thread
    for (auto& consumer_thread : consumers) {
        consumer_thread.join();
    }

    return 0;
}