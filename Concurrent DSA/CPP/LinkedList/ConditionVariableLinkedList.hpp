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

#ifndef CONDITION_VARIABLE_LINKED_LIST_HPP
#define CONDITION_VARIABLE_LINKED_LIST_HPP

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

#endif // CONDITION_VARIABLE_LINKED_LIST_HPP
