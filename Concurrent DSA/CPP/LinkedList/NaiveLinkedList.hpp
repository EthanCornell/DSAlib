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

#ifndef NAIVE_LINKED_LIST_HPP
#define NAIVE_LINKED_LIST_HPP

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

#endif // NAIVE_LINKED_LIST_HPP
