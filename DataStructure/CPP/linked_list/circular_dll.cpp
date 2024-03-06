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
using namespace std;

// Node Class: Represents each element in the list with data, prev, and next pointers.
// CircularDoublyLinkedList Class: Manages the list with insertFront, insertEnd, deleteNode, and display methods.
// Insertion: Nodes can be inserted at the front or end. The new nodes are linked appropriately.
// Deletion: The specified node is searched and removed from the list.
// Display: Traverses and prints the data of each node.
// Destructor: Frees the memory allocated for nodes to prevent memory leaks.
// Complexity Analysis
// Time Complexity:
// Insertion: O(1) as we directly insert at the front or end.
// Deletion: O(n) in the worst case, as we may need to traverse the entire list.
// Display: O(n) as it traverses the entire list.
// Space Complexity: O(n) for storing n nodes in the list.

// Node class
class Node
{
public:
    int data;
    Node *prev, *next;

    Node(int val) : data(val), prev(nullptr), next(nullptr) {}
};

// Circular Doubly Linked List class
class CircularDoublyLinkedList
{
    Node *head;

public:
    CircularDoublyLinkedList() : head(nullptr) {}

    // Insert a new node at the front
    void insertFront(int val)
    {
        Node *newNode = new Node(val);
        if (!head)
        {
            newNode->next = newNode->prev = newNode;
            head = newNode;
            return;
        }
        Node *tail = head->prev;
        newNode->next = head;
        newNode->prev = tail;
        head->prev = tail->next = newNode;
        head = newNode;
    }

    // Insert a new node at the end
    void insertEnd(int val)
    {
        if (!head)
        {
            insertFront(val);
            return;
        }
        Node *newNode = new Node(val);
        Node *tail = head->prev;
        newNode->next = head;
        newNode->prev = tail;
        tail->next = head->prev = newNode;
    }

    // Delete a node with a specific value
    void deleteNode(int val)
    {
        if (!head)
            return;
        Node *current = head;
        do
        {
            if (current->data == val)
            {
                if (current == head && current->next == head)
                {
                    delete current;
                    head = nullptr;
                    return;
                }
                current->prev->next = current->next;
                current->next->prev = current->prev;
                if (current == head)
                    head = current->next;
                delete current;
                return;
            }
            current = current->next;
        } while (current != head);
    }

    // Complexity Analysis of Search Function
    // Time Complexity: O(n), as it might need to traverse the entire list in the worst case.
    // Space Complexity: O(1), as no additional space is used.
    // Search for a node with a specific value
    bool search(int val)
    {
        if (!head)
            return false;
        Node *current = head;
        do
        {
            if (current->data == val)
            {
                return true;
            }
            current = current->next;
        } while (current != head);
        return false;
    }

    // Display the list
    void display()
    {
        if (!head)
            return;
        Node *current = head;
        do
        {
            cout << current->data << " ";
            current = current->next;
        } while (current != head);
        cout << endl;
    }

    // Destructor to free memory
    ~CircularDoublyLinkedList()
    {
        if (!head)
            return;
        Node *current = head;
        do
        {
            Node *next = current->next;
            delete current;
            current = next;
        } while (current != head);
    }
};

// Main function
int main()
{
    CircularDoublyLinkedList list;
    list.insertFront(3);
    list.insertFront(2);
    list.insertFront(1);
    list.insertEnd(4);

    cout << "List after insertion: ";
    list.display();

    // Test search function
    int searchValue = 3;
    cout << "Searching for " << searchValue << ": ";
    cout << (list.search(searchValue) ? "Found" : "Not Found") << endl;

    searchValue = 5;
    cout << "Searching for " << searchValue << ": ";
    cout << (list.search(searchValue) ? "Found" : "Not Found") << endl;

    // Test delete function
    list.deleteNode(3);
    cout << "List after deleting 3: ";
    list.display();

    return 0;
}

// g++ -pg -fopenmp -fsanitize=address -g -std=c++17 -o cdll ./circular_dll.cpp && ./cdll
