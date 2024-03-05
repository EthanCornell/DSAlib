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

// Node class definition
template <typename T>
class Node {
public:
    T data; // Data held by the Node
    std::shared_ptr<Node<T>> next; // Pointer to the next node
    std::shared_ptr<Node<T>> prev; // Pointer to the previous node

    // Constructor initializing Node with a value and null pointers to next and prev
    Node(const T& val) : data(val), next(nullptr), prev(nullptr) {}
};

// CircularDoublyLinkedList class definition
template <typename T>
class CircularDoublyLinkedList {
private:
    std::shared_ptr<Node<T>> head; // Pointer to the head node
    size_t size; // Added size member variable to track the list size

public:
    CircularDoublyLinkedList() : head(nullptr) {} // Constructor initializing an empty list

    // Method to add a node at the front of the list
    void push_front(const T& value);

    // Method to add a node at the back of the list
    void push_back(const T& val);

    // Method to remove a node from the back of the list
    void pop_back();

    // Method to remove a node from the front of the list
    void pop_front();

    // Method to insert a node at a specific index
    void insert(int index, const T& value);

    // Method to delete a node containing a specific value (declaration only)
    void delete_node(const T& value);

    // Method to display the list contents
    void display();
};

// Definitions of CircularDoublyLinkedList methods

// push_front: Add a new node at the beginning of the list.
template<typename T>
void CircularDoublyLinkedList<T>::push_front(const T& value) {
    std::shared_ptr<Node<T>> newNode = std::make_shared<Node<T>>(value);
    if (!head) {
        head = newNode;
        newNode->next = newNode;
        newNode->prev = newNode;
    } else {
        newNode->next = head;
        newNode->prev = head->prev;
        head->prev->next = newNode;
        head->prev = newNode;
        head = newNode;
    }
    this->size++; // Update size
}


// push_back, the time complexity is O(1) as we are adding elements at the end.
// push_back: Add a new node at the end of the list.
template<typename T>
void CircularDoublyLinkedList<T>::push_back(const T& val) {
    std::shared_ptr<Node<T>> newNode = std::make_shared<Node<T>>(val);
    if (!head) {
        head = newNode;
        head->next = head;
        head->prev = head;
    } else {
        newNode->next = head;
        newNode->prev = head->prev;
        head->prev->next = newNode;
        head->prev = newNode;
    }
    this->size++; // Update size
}

// pop_back: Remove the last node of the list.
template<typename T>
void CircularDoublyLinkedList<T>::pop_back() {
    if (!head) return;
    if (head == head->next) {
        head = nullptr;
    } else {
        auto newTail = head->prev->prev;
        newTail->next = head;
        head->prev = newTail;
    }
    if (this->size > 0) this->size--; // Update size
}


// pop_front: Remove the first node of the list.
template<typename T>
void CircularDoublyLinkedList<T>::pop_front() {
    if (!head) return;
    if (head == head->next) {
        head = nullptr;
    } else {
        auto newHead = head->next;
        newHead->prev = head->prev;
        head->prev->next = newHead;
        head = newHead;
    }
    if (this->size > 0) this->size--; // Update size
}



// insert: Insert a new node at a given index.
template<typename T>
void CircularDoublyLinkedList<T>::insert(int index, const T& value) {
    // Assuming 'size' is a member variable that tracks the number of elements in the list
    // This would need to be declared and maintained within the CircularDoublyLinkedList class
    if (index < 0 || index > this->size) {
        throw std::out_of_range("Index out of range");
    }
    if (index == 0) {
        push_front(value);
        return;
    }
    if (index == this->size) {
        push_back(value);
        return;
    }
    std::shared_ptr<Node<T>> newNode = std::make_shared<Node<T>>(value);
    auto current = head;
    for (int i = 0; i < index - 1; i++) {
        current = current->next;
    }
    newNode->next = current->next;
    newNode->prev = current;
    current->next->prev = newNode;
    current->next = newNode;
    // Increment the size of the list
    this->size++;
}

// delete_node: Delete a node with a given value.
template<typename T>
void CircularDoublyLinkedList<T>::delete_node(const T& value) {
    if (!head) return; // List is empty, nothing to do.

    std::shared_ptr<Node<T>> current = head;
    std::shared_ptr<Node<T>> toDelete = nullptr;

    do {
        if (current->data == value) {
            toDelete = current;
            break;
        }
        current = current->next;
    } while (current != head);

    if (!toDelete) return; // Value not found, nothing to do.

    if (toDelete->next == toDelete) {
        // Node is the only node in the list.
        head = nullptr;
    } else {
        toDelete->prev->next = toDelete->next;
        toDelete->next->prev = toDelete->prev;

        if (toDelete == head) {
            // Node is the head, update head to next node.
            head = toDelete->next;
        }
    }

    this->size--; // Decrement the size of the list.
}


template<typename T>
void CircularDoublyLinkedList<T>::display() {
    if (!head) return;
    auto current = head;
    do {
        std::cout << current->data << " ";
        current = current->next;
    } while (current != head);
    std::cout << std::endl;
}


// Time Complexity
// push_front, pop_back, pop_front: O(1)
// insert: O(n) in the worst case (when inserting at the end)
// delete_node: O(n) in the worst case (when the node is at the end or not present)
// Space Complexity
// O(n) for all operations, where n is the number of elements in the list.


int main() {
    CircularDoublyLinkedList<int> list;

    // Test cases
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    std::cout << "List contents: ";
    list.display(); // Expected: 1 2 3

    // Test push_front and push_back
    list.push_front(9);
    list.push_back(2);
    list.push_front(0);
    list.display(); // Expected: 0 9 1 2 3 2

    // Test pop_back and pop_front
    list.pop_back();
    list.pop_front();
    list.display(); // Expected: 9 1 2 3 

    // Test insert
    list.insert(1, 8);
    list.display(); // Expected: 9 8 1 2 3

    // Test delete_node
    list.delete_node(1);
    list.display(); // Expected: 9 8 2 3

    // Add more test cases as needed

    return 0;
}

// g++ -pg -fsanitize=address -g ./circular_dll_smartptr.cpp -o cdllptr -O3