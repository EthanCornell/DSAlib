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

template<typename T>
class Node {
public:
    T value;
    std::unique_ptr<Node<T>> next;

    Node(T val) : value(val), next(nullptr) {}
};

template<typename T>
class LinkedList {
private:
    std::unique_ptr<Node<T>> head;

public:
    LinkedList() : head(nullptr) {}

    void pushFront(T value) {
        auto newNode = std::make_unique<Node<T>>(value);
        newNode->next = std::move(head);
        head = std::move(newNode);
    }

    void popFront() {
        if (head != nullptr) {
            head = std::move(head->next);
        }
    }

    bool find(const T& value) {
        Node<T>* current = head.get();
        while (current != nullptr) {
            if (current->value == value) {
                return true;
            }
            current = current->next.get();
        }
        return false;
    }

    void popBack() {
        if (head == nullptr) {
            return;
        }
        if (head->next == nullptr) {
            head = nullptr;
            return;
        }
        Node<T>* current = head.get();
        while (current->next->next != nullptr) {
            current = current->next.get();
        }
        current->next = nullptr;
    }

    void clear() {
        while (head != nullptr) {
            head = std::move(head->next);
        }
    }

    void printList() {
        Node<T>* current = head.get();
        while (current != nullptr) {
            std::cout << current->value << " ";
            current = current->next.get();
        }
        std::cout << std::endl;
    }
};


// Complexity Analysis
// popFront: O(1) - Removing the first element is a constant time operation.
// find: O(n) - In the worst case, we may have to traverse the entire list.
// popBack: O(n) - Since it's a singly linked list, we need to traverse it to find the second-to-last node.
// clear: O(n) - We need to go through each element to clear the list.
// The space complexity for the list remains O(n), where n is the number of elements in the list.

int main() {
    LinkedList<int> list;
    list.pushFront(10);
    list.pushFront(20);
    list.pushFront(30);

    std::cout << "Initial List: ";
    list.printList();

    list.popFront();
    std::cout << "After popFront: ";
    list.printList();

    std::cout << "Finding 20: " << (list.find(20) ? "Found" : "Not Found") << std::endl;

    list.popBack();
    std::cout << "After popBack: ";
    list.printList();

    list.clear();
    std::cout << "After clear: ";
    list.printList();

    return 0;
}
