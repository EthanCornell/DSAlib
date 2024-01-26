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

#include <iostream>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <queue>

class bounded_queue {
private:
    std::mutex mtx;
    std::condition_variable cv_produce, cv_consume;
    std::queue<int> queue;
    unsigned int size;

public:
    bounded_queue(unsigned int size) : size(size) {}

    void put(int value) {
        std::unique_lock<std::mutex> lock(mtx);
        cv_produce.wait(lock, [this] { return queue.size() < size; });
        queue.push(value);
        cv_consume.notify_one();
    }

    int get() {
        std::unique_lock<std::mutex> lock(mtx);
        cv_consume.wait(lock, [this] { return !queue.empty(); });
        int value = queue.front();
        queue.pop();
        cv_produce.notify_one();
        return value;
    }
};

std::shared_ptr<bounded_queue> bounded_queue_init(unsigned int size) {
    return std::make_shared<bounded_queue>(size);
}

void bounded_queue_put(std::shared_ptr<bounded_queue> &q, int value) {
    q->put(value);
}

int bounded_queue_get(std::shared_ptr<bounded_queue> &q) {
    return q->get();
}

void bounded_queue_destroy(std::shared_ptr<bounded_queue> &q) {
    q.reset();
}

int main() {
    auto queue = bounded_queue_init(5);

    // Example usage
    bounded_queue_put(queue, 1);
    std::cout << "Item produced: 1" << std::endl;

    int item = bounded_queue_get(queue);
    std::cout << "Item consumed: " << item << std::endl;

    bounded_queue_destroy(queue);

    return 0;
}
