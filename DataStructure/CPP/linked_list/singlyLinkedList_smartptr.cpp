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

// Template class for a node in the linked list
template<typename T>
class Node {
public:
    T value;  // Value stored in the node
    std::unique_ptr<Node<T>> next;  // Unique pointer to the next node
    Node(T val) : value(val), next(nullptr) {} // Constructor to initialize a node with a value; next is initialized as nullptr
};

// Template class for the linked list
template<typename T>
class LinkedList {
private:
    std::unique_ptr<Node<T>> head;  // Unique pointer to the head node of the list

public:
    // Constructor initializes the head as nullptr, indicating an empty list
    LinkedList() : head(nullptr) {}

    // Function to add a node at the front of the list
    void pushFront(T value) {
        auto newNode = std::make_unique<Node<T>>(value); // Create a new node
        newNode->next = std::move(head); // Move the current head to the next of new node
        head = std::move(newNode); // Set the new node as the new head
    }

    // Function to remove the front node of the list
    void popFront() {
        if (head != nullptr) { // Check if the list is not empty
            head = std::move(head->next); // Move the second node to become the new head
        }
    }

    // Function to find a value in the list
    bool find(const T& value) {
        Node<T>* current = head.get(); // Start with the head of the list
        while (current != nullptr) { // Iterate until the end of the list
            if (current->value == value) { // Check if the current node contains the value
                return true; // Value found
            }
            current = current->next.get(); // Move to the next node
        }
        return false; // Value not found
    }

    // Function to remove the last node from the list
    void popBack() {
        if (head == nullptr) { // Check if the list is empty
            return; // Nothing to pop
        }
        if (head->next == nullptr) { // Check if the list has only one node
            head = nullptr; // Remove the only node
            return;
        }
        Node<T>* current = head.get(); // Start with the head of the list
        while (current->next->next != nullptr) { // Iterate until the second-last node
            current = current->next.get(); // Move to the next node
        }
        current->next = nullptr; // Remove the last node
    }

    // Function to clear the entire list
    void clear() {
        while (head != nullptr) { // Iterate until the list is empty
            head = std::move(head->next); // Continuously remove the head node
        }
    }

    // Function to print all values in the list
    void printList() {
        Node<T>* current = head.get(); // Start with the head of the list
        while (current != nullptr) { // Iterate until the end of the list
            std::cout << current->value << " "; // Print the value of the current node
            current = current->next.get(); // Move to the next node
        }
        std::cout << std::endl; // Print a newline at the end
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
