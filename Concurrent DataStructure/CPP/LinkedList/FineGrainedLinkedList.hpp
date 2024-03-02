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

// FineGrainedLinkedList.hpp
#ifndef FINE_GRAINED_LINKED_LIST_HPP
#define FINE_GRAINED_LINKED_LIST_HPP

#include <iostream>
#include <memory> // For std::unique_ptr
#include <mutex>  // For std::mutex and std::lock_guard

template<typename T>
class FineGrainedLinkedList {
public:
    struct Node {
        T value;
        std::unique_ptr<Node> next;
        std::mutex mutex;

        Node(T val) : value(val), next(nullptr) {}
    };

    FineGrainedLinkedList() : head(nullptr) {}

    void insert(T value) {
        auto newNode = std::make_unique<Node>(value);
        std::lock_guard<std::mutex> lockHead(headMutex);

        if (!head || head->value > value) {
            newNode->next = std::move(head);
            head = std::move(newNode);
            return;
        }

        Node* prev = head.get();
        prev->mutex.lock();
        Node* current = prev->next.get();

        while (current) {
            current->mutex.lock();
            if (current->value > value) {
                break;
            }
            prev->mutex.unlock();
            prev = current;
            current = current->next.get();
        }

        newNode->next = std::move(prev->next);
        prev->next = std::move(newNode);
        prev->mutex.unlock();
        if (current) current->mutex.unlock();
    }

    bool search(T value) {
        std::lock_guard<std::mutex> lockHead(headMutex);
        Node* current = head.get();
        while (current) {
            std::lock_guard<std::mutex> lockCurrent(current->mutex);
            if (current->value == value) {
                return true;
            }
            current = current->next.get();
        }
        return false;
    }

    bool remove(T value) {
        std::lock_guard<std::mutex> lockHead(headMutex);
        if (!head) return false;

        if (head->value == value) {
            head = std::move(head->next);
            return true;
        }

        Node* prev = head.get();
        prev->mutex.lock();
        Node* current = prev->next.get();

        while (current) {
            current->mutex.lock();
            if (current->value == value) {
                prev->next = std::move(current->next);
                prev->mutex.unlock();
                current->mutex.unlock();
                return true;
            }
            prev->mutex.unlock();
            prev = current;
            current = current->next.get();
        }

        if (prev) prev->mutex.unlock();
        if (current) current->mutex.unlock();
        return false;
    }

private:
    std::unique_ptr<Node> head;
    std::mutex headMutex; // Protects modifications to the head pointer
};

#endif // FINE_GRAINED_LINKED_LIST_HPP
