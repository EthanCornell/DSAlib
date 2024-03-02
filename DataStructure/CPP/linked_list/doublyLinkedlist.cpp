/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
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
 * Project: Cache Replacement Algorithms
 */
#include <iostream>
using namespace std;
struct Node {
    int data;
    Node* prev;
    Node* next;

    Node(int value) : data(value), prev(nullptr), next(nullptr) {}
};

class DoublyLinkedList {
private:
    Node* head;
    Node* tail;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}

    // Function declarations
    void insertFront(int value);
    void insertBack(int value);
    void deleteNode(Node* nodeToDelete);
    Node* search(int value);
    void displayForward();
    void displayBackward();

    // Destructor to free memory
    ~DoublyLinkedList();
};

// Insertion at Front
// Time Complexity: O(1)
// Space Complexity: O(1)
void DoublyLinkedList::insertFront(int value) {
    Node* newNode = new Node(value);
    if (head == nullptr) {
        head = tail = newNode;
    } else {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
}

// Insertion at Back
// Time Complexity: O(1)
// Space Complexity: O(1)
void DoublyLinkedList::insertBack(int value) {
    Node* newNode = new Node(value);
    if (tail == nullptr) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
}

// Deletion
// Time Complexity: O(1)
// Space Complexity: O(1)
void DoublyLinkedList::deleteNode(Node* nodeToDelete) {
    if (nodeToDelete == nullptr) return;

    if (nodeToDelete == head) {
        head = head->next;
    }
    if (nodeToDelete == tail) {
        tail = tail->prev;
    }
    if (nodeToDelete->next != nullptr) {
        nodeToDelete->next->prev = nodeToDelete->prev;
    }
    if (nodeToDelete->prev != nullptr) {
        nodeToDelete->prev->next = nodeToDelete->next;
    }

    delete nodeToDelete;
}


// Search
// Time Complexity: O(n)
// Space Complexity: O(1)
Node* DoublyLinkedList::search(int value) {
    Node* current = head;
    while (current != nullptr) {
        if (current->data == value) return current;
        current = current->next;
    }
    return nullptr;
}

// Display (Forward and Backward)
// Time Complexity: O(n)
// Space Complexity: O(1)
void DoublyLinkedList::displayForward() {
    Node* current = head;
    while (current != nullptr) {
        std::cout << current->data << " ";
        current = current->next;
    }
    std::cout << std::endl;
}

void DoublyLinkedList::displayBackward() {
    Node* current = tail;
    while (current != nullptr) {
        std::cout << current->data << " ";
        current = current->prev;
    }
    std::cout << std::endl;
}

// Destructor
// Time Complexity: O(n)
// Space Complexity: O(1)
DoublyLinkedList::~DoublyLinkedList() {
    Node* current = head;
    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}

int main() {
    DoublyLinkedList dll;
    dll.insertFront(10);
    dll.insertFront(20);
    dll.insertBack(30);
    dll.displayForward();  // 20 10 30

    Node* found = dll.search(10);
    if (found != nullptr) {
        dll.deleteNode(found);
    }
    dll.displayForward();  // 20 30

    return 0;
}

