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

template <typename T>
struct Node
{
    T data;
    std::weak_ptr<Node<T>> prev;
    std::shared_ptr<Node<T>> next;

    explicit Node(const T &val) : data(val), prev(), next() {}
};

template <typename T>
class DoublyLinkedList
{
private:
    std::shared_ptr<Node<T>> head;
    std::weak_ptr<Node<T>> tail;
    size_t size;

public:
    DoublyLinkedList() : head(nullptr), size(0) {}
    // Custom Copy Constructor
    DoublyLinkedList(const DoublyLinkedList<T> &copy);
    void push_back(const T &val);
    void push_front(const T &value);
    void delete_front();
    void delete_back();
    void display_forward();
    void display_backward();
    void delete_node(const T &value);
    bool addAll(const std::initializer_list<T> &elements);
    bool addAll(int index, const std::initializer_list<T> &elements);
    void clear();
    std::unique_ptr<DoublyLinkedList<T>> clone() const;
    bool contains(const T &value) const;

    T element() const;
    T get(int index) const;
};

template <typename T>
DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList<T> &copy) : head(nullptr), tail(), size(0)
{
    auto current = copy.head;
    while (current != nullptr)
    {
        this->push_back(current->data); // Use push_back to copy each element
        current = current->next;
    }
}

template <typename T>
void DoublyLinkedList<T>::push_back(const T &val)
{
    auto newNode = std::make_shared<Node<T>>(val);
    if (!head)
    { // If the list is empty
        head = newNode;
        tail = newNode;
    }
    else
    {
        auto lastNode = tail.lock(); // Get the current tail
        lastNode->next = newNode;    // Point the old tail's next to the new node
        newNode->prev = lastNode;    // Point the new node's prev to the old tail
        tail = newNode;              // Update the tail to the new node
    }
    ++size;
}

template <typename T>
void DoublyLinkedList<T>::push_front(const T &value)
{
    auto newNode = std::make_shared<Node<T>>(value);
    if (!head)
    { // If the list is empty
        head = newNode;
        tail = newNode;
    }
    else
    {
        newNode->next = head; // Make new node point to current head
        head->prev = newNode; // Make current head's previous point to new node
        head = newNode;       // Update head to be the new node
    }
    ++size;
}

template <typename T>
void DoublyLinkedList<T>::delete_front()
{
    if (!head)
    { // If the list is empty, do nothing
        return;
    }
    if (head->next)
    {                                             // If there's more than one node in the list
        auto newHead = head->next;                // The next node becomes the new head
        newHead->prev = std::weak_ptr<Node<T>>(); // Correctly reset the prev pointer of the new head
        head = newHead;                           // Update head to the new head
    }
    else
    {                 // If there's only one node in the list
        head.reset(); // Reset head, effectively deleting the only node
        tail.reset(); // Reset tail as well
    }
    --size; // Decrement the size of the list
}

template <typename T>
void DoublyLinkedList<T>::delete_back()
{
    if (!head)
    { // If the list is empty, do nothing
        return;
    }
    if (!head->next)
    {                 // If there's only one node in the list
        head.reset(); // Delete the only node
        tail.reset(); // Both head and tail are now nullptr
    }
    else
    {
        auto oldTail = tail.lock();          // Get a shared_ptr to the current tail node
        auto newTail = oldTail->prev.lock(); // Get the node before the current tail
        newTail->next.reset();               // Remove the last node by resetting its next pointer
        tail = newTail;                      // Update tail to the new last node
    }
    --size; // Decrement the size of the list
}

template <typename T>
void DoublyLinkedList<T>::delete_node(const T &value)
{
    if (!head)
        return; // If the list is empty, there's nothing to delete

    auto current = head;
    while (current != nullptr && current->data != value)
    {
        current = current->next; // Move to the next node until the value is found or the list ends
    }

    if (!current)
        return; // Value not found in the list

    if (current == head)
    { // If the node to delete is the head
        if (head->next)
        {                                          // If there's a node after head
            head = head->next;                     // Move head to the next node
            head->prev = std::weak_ptr<Node<T>>(); // Remove old head's prev link
        }
        else
        {                 // If it's the only node in the list
            head.reset(); // Clear the list
            tail.reset(); // Both head and tail become nullptr
        }
    }
    else if (!current->next)
    {                                        // If the node to delete is the tail
        auto newTail = current->prev.lock(); // Get the new tail (node before current)
        newTail->next.reset();               // Remove the last node
        tail = newTail;                      // Update the tail
    }
    else
    { // If the node is in the middle
        auto prevNode = current->prev.lock();
        auto nextNode = current->next;
        prevNode->next = nextNode; // Bypass the current node
        nextNode->prev = prevNode; // Correctly set the prev pointer of the next node
    }

    --size; // Decrement the size of the list since a node was deleted
}

template <typename T>
bool DoublyLinkedList<T>::addAll(int index, const std::initializer_list<T> &elements)
{
    if (index < 0 || index > size)
    {                 // Check for valid index
        return false; // Return false if index is out of bounds
    }

    if (index == size)
    { // If adding at the end, utilize existing push_back functionality
        for (const auto &element : elements)
        {
            push_back(element);
        }
        return true;
    }

    auto current = head;
    for (int i = 0; i < index && current != nullptr; ++i)
    {
        current = current->next; // Traverse to the position at index
    }

    for (const auto &element : elements)
    {
        auto newNode = std::make_shared<Node<T>>(element); // Create a new node for each element
        if (current)
        {
            // If inserting in the middle or at the beginning
            newNode->next = current;
            if (current->prev.lock())
            { // If not inserting at the beginning
                auto prevNode = current->prev.lock();
                newNode->prev = prevNode;
                prevNode->next = newNode;
            }
            else
            { // If inserting at the beginning
                head = newNode;
            }
            current->prev = newNode;
            current = newNode->next; // Move current to point to the next node in the original list
        }
        else
        {
            // This condition is a safety net and should not be reached due to the initial index check
            return false;
        }
    }

    size += elements.size(); // Update the size of the list
    return true;
}

template <typename T>
bool DoublyLinkedList<T>::addAll(const std::initializer_list<T> &elements)
{
    for (const auto &element : elements)
    {
        push_back(element); // Use the existing push_back method to add each element
    }
    return true; // Assuming operation is always successful, return true
}

template <typename T>
void DoublyLinkedList<T>::clear()
{
    head.reset(); // Automatically deletes the entire list due to shared_ptr's automatic memory management
    tail.reset(); // Resetting tail as well for completeness, although not strictly necessary after head.reset()
    size = 0;     // Reset size to 0
}

template <typename T>
std::unique_ptr<DoublyLinkedList<T>> DoublyLinkedList<T>::clone() const
{
    auto newList = std::make_unique<DoublyLinkedList<T>>(); // Create a new empty list
    auto current = head;
    while (current != nullptr)
    {
        newList->push_back(current->data); // Use push_back to copy each element
        current = current->next;
    }
    return newList; // Return the new list encapsulated in a unique_ptr
}

template <typename T>
bool DoublyLinkedList<T>::contains(const T &value) const
{
    auto current = head;
    while (current != nullptr)
    {
        if (current->data == value)
            return true; // Value found
        current = current->next;
    }
    return false; // Value not found
}

template <typename T>
T DoublyLinkedList<T>::element() const
{
    if (!head)
    {
        // Handle the empty list case. Throwing an exception is a common approach.
        // throw std::runtime_error("Attempted to access element in an empty list");
        return T{}; // Return a default-constructed object of T. This assumes T can be default-constructed.
    }
    return head->data; // Return the data of the head element
}

template <typename T>
T DoublyLinkedList<T>::get(int index) const
{
    if (index < 0 || index >= size)
    {
        // Handle the invalid index case. Throwing an exception is a common approach.
        // throw std::out_of_range("Index out of range");
        return T{}; // Return a default-constructed object of T. This assumes T can be default-constructed.
    }

    auto current = head;
    for (int i = 0; i < index && current != nullptr; ++i)
    {
        current = current->next; // Traverse to the desired index
    }

    if (current)
    {
        return current->data; // Return the data at the index
    }

    // This point should never be reached due to the earlier range check
    return T{}; // Included for completeness
}

template <typename T>
void DoublyLinkedList<T>::display_forward()
{
    auto current = head;
    while (current != nullptr)
    {
        std::cout << current->data << " ";
        current = current->next;
    }
    std::cout << std::endl;
}

template <typename T>
void DoublyLinkedList<T>::display_backward()
{
    auto current = tail.lock(); // Start from the tail of the list
    while (current)
    { // Continue until you reach a nullptr
        std::cout << current->data << " ";
        current = current->prev.lock(); // Move to the previous node
    }
    std::cout << std::endl; // Print a newline at the end
}

#include <iostream>
#include <cassert> // For assert()

int main()
{
    DoublyLinkedList<int> list;

    // Test basic operations
    list.push_back(10);
    list.push_front(5);
    assert(list.element() == 5); // First element should be 5
    assert(list.get(1) == 10);   // Second element should be 10

    // Test adding multiple elements
    list.addAll({15, 20, 25});
    list.addAll(2, {12, 14}); // Insert in the middle

    // Test contains
    assert(list.contains(12) == true);
    assert(list.contains(99) == false); // Should not be found

    // Test clone
    auto clonedList = list.clone();
    assert(clonedList->contains(12) == true);
    clonedList->delete_node(12);
    assert(clonedList->contains(12) == false);
    assert(list.contains(12) == true); // Ensure original list is unaffected

    // Test display methods (visual inspection)
    std::cout << "Original list forward: ";
    list.display_forward();
    std::cout << "Original list backward: ";
    list.display_backward();

    std::cout << "Cloned list forward (after deletion): ";
    clonedList->display_forward();

    // Test deletion methods
    list.delete_front();
    assert(list.element() != 5); // 5 was at the front, should be removed
    list.delete_back();
    assert(!list.contains(25)); // 25 was at the back, should be removed

    // Test delete_node
    list.delete_node(14); // Delete from middle
    assert(!list.contains(14));

    // Test clear
    list.clear();
    assert(list.element() == int{}); // Should be default value after clear

    std::cout << "All tests passed successfully.\n";

    return 0;
}

// g++ -pg -fopenmp -fsanitize=address -g -std=c++17 -o dllsptr DoublyLinkedList_sptr.cpp -O3 && ./dllsptr
// valgrind --leak-check=full --show-leak-kinds=all ./LFLL