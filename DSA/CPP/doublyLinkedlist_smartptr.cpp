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

#include <memory>
#include <iostream>

template<typename T>
struct Node {
    T data;
    std::unique_ptr<Node<T>> next;
    Node<T>* prev;

    Node(T value) : data(value), next(nullptr), prev(nullptr) {}
};

template<typename T>
class DoublyLinkedList {
private:
    std::unique_ptr<Node<T>> head;
    Node<T>* tail;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}
    ~DoublyLinkedList() {
        while (head) {
            head = std::move(head->next);
        }
    }

    void push_front(T value);
    void push_back(T value);
    void delete_front();
    void delete_back();
    void display_forward();
    void display_backward();
    void delete_node(T value);
    // Other member functions can be added as needed
};


template<typename T>
void DoublyLinkedList<T>::push_front(T value) {
    std::unique_ptr<Node<T>> newNode = std::make_unique<Node<T>>(value);
    newNode->next = std::move(head);
    if (newNode->next) {
        newNode->next->prev = newNode.get();
    } else {
        tail = newNode.get();
    }
    head = std::move(newNode);
}

template<typename T>
void DoublyLinkedList<T>::push_back(T value) {
    if (!head) {
        push_front(value);
        return;
    }
    Node<T>* newNode = new Node<T>(value);
    tail->next.reset(newNode);
    newNode->prev = tail;
    tail = newNode;
}

template<typename T>
void DoublyLinkedList<T>::delete_front() {
    if (head) {
        head = std::move(head->next);
        if (head) {
            head->prev = nullptr;
        } else {
            tail = nullptr;
        }
    }
}

template<typename T>
void DoublyLinkedList<T>::delete_back() {
    if (!tail) return;
    if (tail->prev) {
        tail = tail->prev;
        tail->next.reset();
    } else {
        head.reset();
        tail = nullptr;
    }
}


// Time and Space Complexity
// The time complexity of delete_node is O(n) because, in the worst case, it needs to traverse the entire list to find the node.
// The space complexity remains O(n) for the list.
template<typename T>
void DoublyLinkedList<T>::delete_node(T value) {
    if (!head) return; // List is empty

    // If the head needs to be deleted
    if (head->data == value) {
        delete_front();
        return;
    }

    Node<T>* current = head.get();
    while (current != nullptr && current->data != value) {
        current = current->next.get();
    }

    // If the node was not found
    if (current == nullptr) return;

    // If the tail needs to be deleted
    if (current == tail) {
        delete_back();
        return;
    }

    // Delete a middle node
    Node<T>* prevNode = current->prev;
    std::unique_ptr<Node<T>> nextNode = std::move(current->next);

    prevNode->next = std::move(nextNode);
    prevNode->next->prev = prevNode;
}



template<typename T>
void DoublyLinkedList<T>::display_forward() {
    for (Node<T>* curr = head.get(); curr != nullptr; curr = curr->next.get()) {
        std::cout << curr->data << " ";
    }
    std::cout << std::endl;
}

template<typename T>
void DoublyLinkedList<T>::display_backward() {
    for (Node<T>* curr = tail; curr != nullptr; curr = curr->prev) {
        std::cout << curr->data << " ";
    }
    std::cout << std::endl;
}

// Time and Space Complexity
// For push_front and push_back, the time complexity is O(1) as we are adding elements at the ends.
// For delete_front and delete_back, the time complexity is also O(1) since no traversal is needed.
// The space complexity for the entire list is O(n), where n is the number of elements in the list.
int main() {
    DoublyLinkedList<int> list;

    // Test cases1
    list.push_front(1);
    list.push_front(2);
    list.push_back(3);
    list.push_back(4);

    std::cout << "List in forward order: ";
    list.display_forward();

    std::cout << "List in backward order: ";
    list.display_backward();

    list.delete_front();
    list.delete_back();

    std::cout << "List after popping front and back: ";
    list.display_forward();

    // Test cases2
    list.push_front(1);
    list.push_front(2);
    list.push_back(3);
    list.push_back(4);

    std::cout << "Initial List: ";
    list.display_forward();

    // Deleting nodes
    list.delete_node(2); // Deleting head
    list.delete_node(4); // Deleting tail
    list.delete_node(3); // Deleting a middle node

    std::cout << "List after deletions: ";
    list.display_forward();

    return 0;
}
