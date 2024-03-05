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
#include <stdexcept>
#include <initializer_list>

template <typename T>
struct Node
{
    T data;
    std::unique_ptr<Node<T>> next;
    Node<T> *prev;

    Node(T value) : data(value), next(nullptr), prev(nullptr) {}
};

template <typename T>
class DoublyLinkedList
{
private:
    std::unique_ptr<Node<T>> head;
    Node<T> *tail;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}
    ~DoublyLinkedList()
    {
        while (head)
        {
            head = std::move(head->next);
        }
    }

    // Custom Copy Constructor
    DoublyLinkedList(const DoublyLinkedList<T> &other) : head(nullptr), tail(nullptr)
    {
        Node<T> *current = other.head.get();
        while (current != nullptr)
        {
            push_back(current->data);
            current = current->next.get();
        }
    }

    void push_front(T value);
    void push_back(T value);
    void delete_front();
    void delete_back();
    void display_forward();
    void display_backward();
    void delete_node(T value);
    bool addAll(const std::initializer_list<T> &elements);
    bool addAll(int index, const std::initializer_list<T> &elements);
    void clear();
    std::unique_ptr<DoublyLinkedList<T>> clone() const;
    bool contains(const T &value) const;

    T element() const;
    T get(int index) const;

    class DescendingIterator
    {
    private:
        Node<T> *current;

    public:
        explicit DescendingIterator(Node<T> *start) : current(start) {}

        bool hasNext() const
        {
            return current != nullptr;
        }

        T next()
        {
            if (!hasNext())
                throw std::out_of_range("No more elements");
            T data = current->data;
            current = current->prev;
            return data;
        }
    };

    DescendingIterator descendingIterator() const
    {
        return DescendingIterator(tail);
    }
    // Other member functions can be added as needed
};

template <typename T>
void DoublyLinkedList<T>::push_front(T value)
{
    std::unique_ptr<Node<T>> newNode = std::make_unique<Node<T>>(value);
    newNode->next = std::move(head);
    if (newNode->next)
    {
        newNode->next->prev = newNode.get();
    }
    else
    {
        tail = newNode.get();
    }
    head = std::move(newNode);
}

template <typename T>
void DoublyLinkedList<T>::push_back(T value)
{
    if (!head)
    {
        push_front(value);
        return;
    }
    Node<T> *newNode = new Node<T>(value);
    tail->next.reset(newNode);
    newNode->prev = tail;
    tail = newNode;
}

template <typename T>
void DoublyLinkedList<T>::delete_front()
{
    if (head)
    {
        head = std::move(head->next);
        if (head)
        {
            head->prev = nullptr;
        }
        else
        {
            tail = nullptr;
        }
    }
}

template <typename T>
void DoublyLinkedList<T>::delete_back()
{
    if (!tail)
        return;
    if (tail->prev)
    {
        tail = tail->prev;
        tail->next.reset();
    }
    else
    {
        head.reset();
        tail = nullptr;
    }
}

// Time and Space Complexity
// The time complexity of delete_node is O(n) because, in the worst case, it needs to traverse the entire list to find the node.
// The space complexity remains O(n) for the list.
template <typename T>
void DoublyLinkedList<T>::delete_node(T value)
{
    if (!head)
        return; // List is empty

    // If the head needs to be deleted
    if (head->data == value)
    {
        delete_front();
        return;
    }

    Node<T> *current = head.get();
    while (current != nullptr && current->data != value)
    {
        current = current->next.get();
    }

    // If the node was not found
    if (current == nullptr)
        return;

    // If the tail needs to be deleted
    if (current == tail)
    {
        delete_back();
        return;
    }

    // Delete a middle node
    Node<T> *prevNode = current->prev;
    std::unique_ptr<Node<T>> nextNode = std::move(current->next);

    prevNode->next = std::move(nextNode);
    prevNode->next->prev = prevNode;
}

// Appending Elements: For appending, we simply iterate over the collection and use the existing push_back function.
// The time complexity for addAll (appending) is O(n) where n is the number of elements in the collection.
template <typename T>
bool DoublyLinkedList<T>::addAll(const std::initializer_list<T> &elements)
{
    for (const auto &element : elements)
    {
        push_back(element);
    }
    return true; // Always returns true as the list is always modified
}

// Inserting at a Position: For inserting at a specific position, we first navigate to the node at the given index, then insert each element from the collection after it.
// The time complexity for addAll(int index, Collection<? extends E> c) is O(n + m) where n is the number of elements to reach the index and m is the number of elements in the collection.
template <typename T>
bool DoublyLinkedList<T>::addAll(int index, const std::initializer_list<T> &elements)
{
    if (index < 0)
        return false; // Invalid index

    if (index == 0)
    {
        for (auto it = elements.end(); it != elements.begin();)
        {
            --it;
            push_front(*it);
        }
        return true;
    }

    Node<T> *current = head.get();
    for (int i = 0; i < index - 1 && current != nullptr; ++i)
    {
        current = current->next.get();
    }

    if (current == nullptr)
        return false; // Index out of bounds

    for (auto it = elements.begin(); it != elements.end(); ++it)
    {
        std::unique_ptr<Node<T>> newNode = std::make_unique<Node<T>>(*it);
        newNode->next = std::move(current->next);
        if (newNode->next)
        {
            newNode->next->prev = newNode.get();
        }
        else
        {
            tail = newNode.get();
        }
        newNode->prev = current;
        current->next = std::move(newNode);
        current = current->next.get();
    }
    return true;
}

template <typename T>
void DoublyLinkedList<T>::clear()
{
    while (head)
    {
        head = std::move(head->next);
    }
    tail = nullptr;
}

template <typename T>
std::unique_ptr<DoublyLinkedList<T>> DoublyLinkedList<T>::clone() const
{
    auto newList = std::make_unique<DoublyLinkedList<T>>();
    for (Node<T> *curr = head.get(); curr != nullptr; curr = curr->next.get())
    {
        newList->push_back(curr->data);
    }
    return newList;
}

template <typename T>
bool DoublyLinkedList<T>::contains(const T &value) const
{
    for (Node<T> *curr = head.get(); curr != nullptr; curr = curr->next.get())
    {
        if (curr->data == value)
        {
            return true;
        }
    }
    return false;
}

template <typename T>
T DoublyLinkedList<T>::element() const
{
    if (!head)
        throw std::out_of_range("List is empty");
    return head->data;
}

template <typename T>
void DoublyLinkedList<T>::display_forward()
{
    for (Node<T> *curr = head.get(); curr != nullptr; curr = curr->next.get())
    {
        std::cout << curr->data << " ";
    }
    std::cout << std::endl;
}

template <typename T>
T DoublyLinkedList<T>::get(int index) const
{
    if (index < 0)
        throw std::out_of_range("Invalid index");
    Node<T> *current = head.get();
    for (int i = 0; i < index; ++i)
    {
        if (!current)
            throw std::out_of_range("Index out of bounds");
        current = current->next.get();
    }
    return current->data;
}

template <typename T>
void DoublyLinkedList<T>::display_backward()
{
    for (Node<T> *curr = tail; curr != nullptr; curr = curr->prev)
    {
        std::cout << curr->data << " ";
    }
    std::cout << std::endl;
}

// Time and Space Complexity
// For push_front and push_back, the time complexity is O(1) as we are adding elements at the ends.
// For delete_front and delete_back, the time complexity is also O(1) since no traversal is needed.
// The space complexity for the entire list is O(n), where n is the number of elements in the list.
int main()
{
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

    // Test addAll at the end
    std::cout << "Adding elements at the end:" << std::endl;
    list.addAll({10, 20, 30});
    list.display_forward(); // Expected: 10 20 30

    // Test addAll at a specific position
    std::cout << "\nAdding elements at position 1:" << std::endl;
    list.addAll(1, {40, 50});
    list.display_forward(); // Expected: 10 40 50 20 30

    // Test addAll at the beginning
    std::cout << "\nAdding elements at the beginning:" << std::endl;
    list.addAll(0, {60, 70});
    list.display_forward(); // Expected: 60 70 10 40 50 20 30

    // Test addAll at the end (again)
    std::cout << "\nAdding more elements at the end:" << std::endl;
    list.addAll({80, 90});
    list.display_forward(); // Expected: 60 70 10 40 50 20 30 80 90

    // Test addAll at an invalid position (larger than list size)
    std::cout << "\nTrying to add elements at an invalid position:" << std::endl;
    bool result = list.addAll(20, {100, 110});
    std::cout << "Operation successful: " << std::boolalpha << result << std::endl;
    list.display_forward(); // Expected: No change in list

    // test 4
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    // Test clear
    list.clear();
    // Should be empty now

    // Test clone
    list.push_back(4);
    list.push_back(5);
    auto clonedList = list.clone();
    clonedList->display_forward(); // Should display the elements of the cloned list
    // clonedList should have the same elements

    // Test contains
    bool contains4 = list.contains(4);                        // Should be true
    bool contains6 = list.contains(6);                        // Should be false
    std::cout << contains4 << ", " << contains6 << std::endl; // true, false

    // Test descendingIterator
    auto it = list.descendingIterator();
    while (it.hasNext())
    {
        std::cout << it.next() << " ";
    }
    std::cout << std::endl; // Should print 5 4

    // Test element
    int firstElement = list.element(); // Should be 4

    // Test get
    int elementAt1 = list.get(1); // Should be 5
    return 0;
}

//  g++ -pg -fsanitize=address -g ./doublyLinkedlist_smartptr.cpp -o dllsptr -O3