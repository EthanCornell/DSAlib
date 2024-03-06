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
class Node
{
public:
    T data;                        // Data held by the Node
    std::shared_ptr<Node<T>> next; // Pointer to the next node
    std::weak_ptr<Node<T>> prev;   // Pointer to the previous node

    // Constructor initializing Node with a value and null pointers to next and prev
    Node(const T &val) : data(val), next(nullptr) {}
};

// CircularDoublyLinkedList class definition
template <typename T>
class CircularDoublyLinkedList
{
private:
    std::shared_ptr<Node<T>> head; // Pointer to the head node

    size_t size; // Added size member variable to track the list size

public:
    CircularDoublyLinkedList() : head(nullptr) {} // Constructor initializing an empty list

    // Method to add a node at the front of the list
    void push_front(const T &value);

    // Method to add a node at the back of the list
    void push_back(const T &val);

    // Method to remove a node from the back of the list
    void pop_back();

    // Method to remove a node from the front of the list
    void pop_front();

    // Method to insert a node at a specific index
    void insert(int index, const T &value);

    // Method to delete a node containing a specific value (declaration only)
    void delete_node(const T &value);

    // Method to display the list contents
    void display();
};

template <typename T>
void CircularDoublyLinkedList<T>::push_front(const T &value)
{
    auto new_node = std::make_shared<Node<T>>(value);

    if (!head)
    { // If the list is empty
        head = new_node;
        head->next = head; // Point to itself, maintaining circular nature
        head->prev = head; // Also point to itself
    }
    else
    {
        // New node's next is the current head
        new_node->next = head;
        // New node's prev is the current head's prev
        new_node->prev = head->prev;
        // Update the previous head's prev to new node
        head->prev.lock()->next = new_node;
        // Update head's prev to new node
        head->prev = new_node;
        // Update head to new node
        head = new_node;
    }

    size++; // Increase list size
}

template <typename T>
void CircularDoublyLinkedList<T>::push_back(const T &val)
{
    auto new_node = std::make_shared<Node<T>>(val);
    if (!head)
    {
        // If the list is empty, this new node becomes the head
        // and both next and prev point to itself.
        head = new_node;
        head->next = head;
        head->prev = head;
    }
    else
    {
        // If the list is not empty, insert the new node at the back.
        // This involves setting the new node's next to head,
        // and its prev to what was previously the last node.
        new_node->next = head;
        new_node->prev = head->prev;

        // Now, we need to update the last node's next to point to this new node
        // and also update head's prev to point to the new node, maintaining the circular nature.
        head->prev.lock()->next = new_node;
        head->prev = new_node;
    }
    size++; // Increment the size of the list.
}

template <typename T>
void CircularDoublyLinkedList<T>::pop_back()
{
    // Check if the list is empty
    if (!head)
    {
        std::cout << "List is already empty." << std::endl;
        return;
    }

    // Check if the list has only one element
    if (head->next == head)
    {
        // If there's only one element, set head to nullptr and decrease the size
        head = nullptr;
        size = 0;
    }
    else
    {
        // If the list has more than one element, adjust pointers to remove the last element
        auto last_node = head->prev.lock();          // Get the last node
        auto new_last_node = last_node->prev.lock(); // Get the new last node (second to last before removal)

        new_last_node->next = head; // Point the new last node's next to head
        head->prev = new_last_node; // Update head's prev to new last node

        // The removed node (last_node) will be destroyed when this function ends,
        // as its shared_ptr goes out of scope and there are no more references to it.

        size--; // Decrease the size of the list
    }
}

template <typename T>
void CircularDoublyLinkedList<T>::pop_front()
{
    // Check if the list is empty
    if (!head)
    {
        std::cout << "List is already empty." << std::endl;
        return;
    }

    // Check if the list has only one element
    if (head->next == head)
    {
        // If there's only one element, simply reset the head to nullptr and decrease the size
        head = nullptr;
        size = 0;
    }
    else
    {
        // If the list has more than one element
        auto new_head = head->next;         // The second node becomes the new head
        new_head->prev = head->prev;        // Update the new head's prev to the last node
        head->prev.lock()->next = new_head; // Update the last node's next to the new head

        // The old head node will be automatically destroyed when this method ends
        // since it's a shared_ptr and there are no more references to it.

        head = new_head; // Update the head pointer to the new head

        size--; // Decrease the size of the list
    }
}

template <typename T>
void CircularDoublyLinkedList<T>::insert(int index, const T &value)
{
    if (index < 0 || index > size)
    {
        std::cout << "Index out of bounds." << std::endl;
        return;
    }

    // If inserting at the front
    if (index == 0)
    {
        push_front(value);
        return;
    }

    // If inserting at the back
    if (index == size)
    {
        push_back(value);
        return;
    }

    auto new_node = std::make_shared<Node<T>>(value);
    auto current = head;

    // Navigate to the position where the new node will be inserted
    for (int i = 0; i < index - 1; ++i)
    {
        current = current->next;
    }

    // Insert the new node
    new_node->next = current->next;
    new_node->prev = current;

    // Adjust the previous node's next pointer
    current->next->prev = new_node;
    current->next = new_node;

    size++; // Increase the size of the list
}

template <typename T>
void CircularDoublyLinkedList<T>::delete_node(const T &value)
{
    if (!head)
    {
        std::cout << "List is empty." << std::endl;
        return;
    }

    std::shared_ptr<Node<T>> current = head;
    std::shared_ptr<Node<T>> toDelete = nullptr;

    // Loop through the list to find the node to delete
    do
    {
        if (current->data == value)
        {
            toDelete = current;
            break;
        }
        current = current->next;
    } while (current != head);

    // If the node was not found
    if (!toDelete)
    {
        std::cout << "Node not found." << std::endl;
        return;
    }

    // If the list has only one node
    if (toDelete->next == toDelete)
    {
        head = nullptr;
    }
    else
    {
        // Adjust the pointers
        toDelete->prev.lock()->next = toDelete->next;
        toDelete->next->prev = toDelete->prev;

        // If the node to delete is the head, move the head pointer
        if (toDelete == head)
        {
            head = toDelete->next;
        }
    }

    // Clear the toDelete node's pointers
    toDelete->next = nullptr;
    toDelete->prev.reset();

    size--; // Decrease the size of the list
}

template <typename T>
void CircularDoublyLinkedList<T>::display()
{
    if (!head)
    {
        std::cout << "List is empty." << std::endl;
        return;
    }
    auto current = head;
    do
    {
        std::cout << current->data << " ";
        current = current->next;
    } while (current != head);
    std::cout << std::endl;
}

int main()
{
    CircularDoublyLinkedList<int> list;

    // Test adding elements to the front
    std::cout << "Testing push_front:" << std::endl;
    list.push_front(3);
    list.push_front(2);
    list.push_front(1);
    list.display(); // Expected: 1 2 3

    // Test adding elements to the back
    std::cout << "\nTesting push_back:" << std::endl;
    list.push_back(4);
    list.push_back(5);
    list.display(); // Expected: 1 2 3 4 5

    // Test inserting elements
    std::cout << "\nTesting insert:" << std::endl;
    list.insert(2, 99); // Insert at middle
    list.display();     // Expected: 1 2 99 3 4 5
    list.insert(0, 88); // Insert at front
    list.display();     // Expected: 88 1 2 99 3 4 5
    list.insert(7, 77); // Insert at back
    list.display();     // Expected: 88 1 2 99 3 4 5 77

    // Test removing elements from the back
    std::cout << "\nTesting pop_back:" << std::endl;
    list.pop_back();
    list.display(); // Expected: 88 1 2 99 3 4 5

    // Test removing elements from the front
    std::cout << "\nTesting pop_front:" << std::endl;
    list.pop_front();
    list.display(); // Expected: 1 2 99 3 4 5

    // Test deleting a specific node
    std::cout << "\nTesting delete_node:" << std::endl;
    list.delete_node(99);
    list.display(); // Expected: 1 2 3 4 5
    list.delete_node(1);
    list.display(); // Expected: 2 3 4 5
    list.delete_node(5);
    list.display(); // Expected: 2 3 4

    // Test edge cases
    std::cout << "\nTesting edge cases:" << std::endl;
    // Removing until empty
    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.display(); // Expected: List is empty.

    // Attempt to remove from empty list
    std::cout << "Attempting to pop from an empty list:" << std::endl;
    list.pop_back();
    list.pop_front();
    list.delete_node(10);

    // Adding to an empty list then removing
    list.push_back(10);
    list.display(); // Expected: 10
    list.pop_front();
    list.display(); // Expected: List is empty.

    return 0;
}

// g++ -pg -fsanitize=address -g ./circular_dll_ptr2.cpp -o cdllptr2 -O3   && valgrind --leak-check=full ./cdllptr2