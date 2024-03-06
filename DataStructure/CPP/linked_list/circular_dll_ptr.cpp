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
    std::weak_ptr<Node<T>> tail;
    size_t size; // Added size member variable to track the list size

public:
    CircularDoublyLinkedList() : head(nullptr) {} // Constructor initializing an empty list

    ~CircularDoublyLinkedList(); // Destructor for cleaning up memory

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
CircularDoublyLinkedList<T>::~CircularDoublyLinkedList()
{
    while (head)
    {
        pop_front();
    }
}

template <typename T>
void CircularDoublyLinkedList<T>::push_front(const T &value)
{
    auto newNode = std::make_shared<Node<T>>(value); // Create a new node with the given value

    if (!head)
    { // List is empty
        head = newNode;
        head->next = head; // Point to itself
        head->prev = head;
        tail = head; // Since it's circular, tail and head are the same for the first element
    }
    else
    {                                // List has one or more elements
        auto lastNode = tail.lock(); // Access the last node using the weak_ptr
        newNode->next = head;        // New node's next points to the current head
        newNode->prev = lastNode;    // New node's prev points to the last node
        head->prev = newNode;        // Current head's prev points to the new node
        if (lastNode)
        {
            lastNode->next = newNode; // Last node's next points to the new node
        }
        head = newNode; // Update head to the new node
    }

    size++; // Increment the size of the list
}

template <typename T>
void CircularDoublyLinkedList<T>::push_back(const T &val)
{
    auto newNode = std::make_shared<Node<T>>(val); // Create a new node with the given value

    if (!head)
    { // List is empty
        head = newNode;
        head->next = head; // Point to itself
        head->prev = head;
        tail = head; // Since it's circular, tail and head are the same for the first element
    }
    else
    {                                // List has one or more elements
        auto lastNode = tail.lock(); // Access the last node using the weak_ptr
        newNode->next = head;        // New node's next points to the head (circularity)
        newNode->prev = lastNode;    // New node's prev points to what was the last node
        head->prev = newNode;        // Head's prev now points to the new node (maintain circularity)
        if (lastNode)
        {
            lastNode->next = newNode; // Last node's next points to the new node
        }
        tail = newNode; // Update the tail to the new node
    }

    size++; // Increment the size of the list
}

template <typename T>
void CircularDoublyLinkedList<T>::pop_back()
{
    if (!head)
    { // List is empty
        size = 0;
        std::cout << "List is already empty." << std::endl;
        return; // Nothing to do
    }
    else if (head->next == head)
    {                   // List has one element
        head = nullptr; // The list is now empty
        tail.reset();   // Explicitly reset tail to release the last node
    }
    else
    {                                // List has more than one element
        auto lastNode = tail.lock(); // Get a shared_ptr to the last node
        if (lastNode)
        {
            auto newLastNode = lastNode->prev.lock(); // Get the second-last node
            if (newLastNode)
            {
                newLastNode->next = head; // Point the second-last node's next to head
                head->prev = newLastNode; // Update head's prev to the new last node
                tail = newLastNode;       // Update the tail pointer
            }
        }
    }

    size--; // Decrement the size of the list
    // Note: If the list becomes empty, size will correctly become 0.
}

template <typename T>
void CircularDoublyLinkedList<T>::pop_front()
{
    if (!head)
    { // List is empty
        size = 0;
        std::cout << "List is already empty." << std::endl;
        return; // Nothing to do
    }
    else if (head->next == head)
    {                   // List has one element
        head = nullptr; // The list is now empty
        tail.reset();   // Explicitly reset the tail to release the last node
    }
    else
    {                                // List has more than one element
        auto newHead = head->next;   // The second node will become the new head
        auto lastNode = tail.lock(); // Get a shared_ptr to the last node
        if (lastNode)
        {
            lastNode->next = newHead; // Update the last node's next to point to the new head
            newHead->prev = lastNode; // Update the new head's prev to point to the last node
        }
        head = newHead; // Update the head pointer to the new head
    }

    size--; // Decrement the size of the list
    // Note: If the list becomes empty, size will correctly become 0.
}

template <typename T>
void CircularDoublyLinkedList<T>::insert(int index, const T &value)
{
    if (index < 0 || index > size)
    { // Check for a valid index
        std::cout << "Index out of bounds" << std::endl;
        return;
    }

    auto newNode = std::make_shared<Node<T>>(value); // Create the new node

    if (index == 0)
    { // Insert at the beginning
        if (!head)
        { // List is empty
            head = newNode;
            head->next = head;
            head->prev = head;
            tail = head; // Since it's circular, tail and head are the same
        }
        else
        { // List is not empty
            newNode->next = head;
            auto lastNode = tail.lock();
            newNode->prev = lastNode;
            head->prev = newNode;
            if (lastNode)
            {
                lastNode->next = newNode;
            }
            head = newNode;
        }
    }
    else if (index == size)
    { // Insert at the end
        auto lastNode = tail.lock();
        newNode->next = head; // Maintain circularity
        newNode->prev = lastNode;
        if (lastNode)
        {
            lastNode->next = newNode;
        }
        head->prev = newNode; // New node becomes the new tail
        tail = newNode;
    }
    else
    { // Insert in the middle
        auto current = head;
        for (int i = 0; i < index - 1; ++i)
        { // Traverse to the position just before the insertion point
            current = current->next;
        }
        newNode->next = current->next;
        newNode->prev = current;
        current->next->prev = newNode; // Update the previous node of the original node at the index
        current->next = newNode;
    }

    size++; // Increment the list size
}

template <typename T>
void CircularDoublyLinkedList<T>::delete_node(const T &value)
{
    if (!head)
    { // List is empty
        size = 0;
        return; // Nothing to do
    }

    std::shared_ptr<Node<T>> current = head;
    std::shared_ptr<Node<T>> prevNode = nullptr;
    bool found = false;
    size_t nodesChecked = 0; // To prevent infinite loop in circular list

    do
    {
        if (current->data == value)
        { // Node with value found
            found = true;
            break;
        }
        prevNode = current;
        current = current->next;
        nodesChecked++;
    } while (current != head && nodesChecked < size);

    if (!found)
    {
        std::cout << "Value not found in the list." << std::endl;
        return; // Value not found, exit function
    }

    if (current->next == current)
    { // Only one node in the list
        head = nullptr;
        tail.reset();
    }
    else
    {
        if (current == head)
        { // Node to delete is the head
            head = head->next;
        }
        auto nextNode = current->next;
        auto prevNode = current->prev.lock();

        if (prevNode)
        {
            prevNode->next = nextNode;
        }
        if (nextNode)
        {
            nextNode->prev = prevNode;
        }

        if (tail.lock() == current)
        { // Node to delete is the tail
            tail = prevNode;
        }
    }

    size--; // Decrement the size of the list
    // Note: current will be automatically deleted due to smart pointers going out of scope
}

template <typename T>
void CircularDoublyLinkedList<T>::display()
{
    if (!head)
        return;
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

// g++ -pg -fsanitize=address -g ./circular_dll_ptr.cpp -o cdllptr -O3   && valgrind --leak-check=full ./cdllptr