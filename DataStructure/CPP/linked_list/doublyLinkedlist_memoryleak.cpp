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

// this code is detected memory leaks

#include <iostream>
#include <memory>

template <typename T>
struct Node
{
    T data;
    std::shared_ptr<Node<T>> prev;
    std::shared_ptr<Node<T>> next; // Change to shared_ptr for ownership

    explicit Node(const T &val) : data(val), prev(nullptr), next(nullptr) {}
};

template <typename T>
class DoublyLinkedList
{
private:
    std::shared_ptr<Node<T>> head;
    std::shared_ptr<Node<T>> tail; // Change to shared_ptr for ownership
    size_t size;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}
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

// copy constructor
template <typename T>
DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList<T> &copy) : head(nullptr), tail(nullptr), size(0)
{
    std::shared_ptr<Node<T>> current = copy.head; // Start from the head of the 'copy' list
    while (current != nullptr)
    {
        this->push_back(current->data); // Copy each element to the new list
        current = current->next;        // Move to the next node
    }
}

template <typename T>
void DoublyLinkedList<T>::push_back(const T &val)
{
    auto newNode = std::make_shared<Node<T>>(val);
    if (!head)
    {
        head = newNode;
        tail = newNode;
    }
    else
    {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    ++this->size;
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
        newNode->next = head; // Point the new node's next to the current head
        head->prev = newNode; // Update the current head's prev to the new node
        head = newNode;       // Update the head of the list to be the new node
    }
    ++this->size;
}

template <typename T>
void DoublyLinkedList<T>::delete_front()
{
    if (!head)
    { // List is empty
        return;
    }

    if (head == tail)
    {                 // List has only one node
        head.reset(); // Automatically deletes the node and sets head to nullptr
        tail.reset(); // Ensure tail is also nullptr
    }
    else
    {
        auto newHead = head->next; // Temporarily store the next node
        newHead->prev.reset();     // Remove the previous link to the old head
        head = newHead;            // Update head to the new head
    }
    this->size = (this->size > 0) ? this->size - 1 : 0; // Ensure size is correctly updated
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
    this->size = (this->size > 0) ? this->size - 1 : 0; // Ensure size is correctly updated
}

template <typename T>
bool DoublyLinkedList<T>::addAll(const std::initializer_list<T> &elements)
{
    for (const auto &element : elements)
    {
        push_back(element); // Use the existing push_back method to add each element
    }
    return true; // Operation successful
}

template <typename T>
bool DoublyLinkedList<T>::addAll(int index, const std::initializer_list<T> &elements)
{
    if (index < 0 || index > this->size)
    {
        // Index is out of bounds
        return false;
    }

    // Special case: adding at the start
    if (index == 0)
    {
        for (auto it = elements.end(); it != elements.begin();)
        {
            push_front(*(--it));
        }
        return true;
    }

    // Special case: adding at the end
    if (index == this->size)
    {
        for (const auto &element : elements)
        {
            push_back(element);
        }
        return true;
    }

    auto current = head;
    for (int i = 0; i < index - 1; ++i)
    {
        current = current->next; // Move to the node after which we'll insert elements
    }

    for (const auto &element : elements)
    {
        auto newNode = std::make_shared<Node<T>>(element);
        newNode->next = current->next;
        if (current->next)
        {
            current->next->prev = newNode;
        }
        current->next = newNode;
        newNode->prev = current;

        if (newNode->next == nullptr)
        {
            // If we're inserting at the end, update the tail
            tail = newNode;
        }
        current = newNode; // Move current to the last inserted node
        this->size++;
    }

    return true;
}

template <typename T>
void DoublyLinkedList<T>::delete_node(const T &value)
{
    auto current = head;
    while (current != nullptr && current->data != value)
    {
        current = current->next; // Move to the next node
    }

    if (current == nullptr)
    {
        // Node with the specified value not found
        return;
    }

    if (current == head)
    {
        // Node is at the front
        delete_front();
    }
    else if (current == tail)
    {
        // Node is at the end
        delete_back();
    }
    else
    {
        // Node is in the middle
        auto prevNode = current->prev;
        auto nextNode = current->next;
        prevNode->next = nextNode; // Bypass the current node
        nextNode->prev = prevNode;
        this->size = (this->size > 0) ? this->size - 1 : 0; // Ensure size is correctly updated
    }
}

template <typename T>
void DoublyLinkedList<T>::clear()
{
    while (head != nullptr)
    {
        head = head->next; // Move head to the next node, which will delete the current node due to shared_ptr's automatic memory management
    }
    // At this point, all nodes have been deleted, and head is nullptr.
    // Explicitly set tail to nullptr to ensure the list is fully cleared.
    tail = nullptr;
    size = 0; // Reset the size of the list to 0.
}

template <typename T>
std::unique_ptr<DoublyLinkedList<T>> DoublyLinkedList<T>::clone() const
{
    auto newList = std::make_unique<DoublyLinkedList<T>>(); // Create a new empty list
    auto current = head;
    while (current != nullptr)
    {
        newList->push_back(current->data); // Use the existing push_back method to add each element
        current = current->next;           // Move to the next node in the original list
    }
    return newList; // Return the newly created list
}

template <typename T>
bool DoublyLinkedList<T>::contains(const T &value) const
{
    auto current = head;
    while (current != nullptr)
    {
        if (current->data == value)
        {
            return true; // Found the value
        }
        current = current->next; // Move to the next node
    }
    return false; // The value is not in the list
}

template <typename T>
T DoublyLinkedList<T>::element() const
{
    if (head == nullptr)
    {
        throw std::out_of_range("The list is empty."); // Or use another way to handle this case
    }
    return head->data;
}

template <typename T>
T DoublyLinkedList<T>::get(int index) const
{
    if (index < 0 || index >= size)
    {
        throw std::out_of_range("Index is out of bounds.");
    }
    auto current = head;
    for (int i = 0; i < index; ++i)
    {
        current = current->next; // Move to the next node
    }
    return current->data;
}

// For testing: display_forward to print elements from head to tail
template <typename T>
void DoublyLinkedList<T>::display_forward()
{
    auto current = head;
    while (current)
    {
        std::cout << current->data << " ";
        current = current->next;
    }
    std::cout << std::endl;
}

template <typename T>
void DoublyLinkedList<T>::display_backward()
{
    auto current = tail; // Start from the tail
    while (current != nullptr)
    {
        std::cout << current->data << " ";
        current = current->prev; // Move to the previous node
    }
    std::cout << std::endl;
}

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

    // Test element
    int firstElement = list.element(); // Should be 4

    // Test get
    int elementAt1 = list.get(1); // Should be 5

    // test for element
    DoublyLinkedList<int> dll;
    dll.push_back(1);
    dll.push_back(2);
    dll.push_back(3);

    // Test the element method
    try
    {
        int firstElement = dll.element();
        std::cout << "The first element is: " << firstElement << std::endl; // Expected: 1
    }
    catch (const std::out_of_range &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    // Test the get method
    try
    {
        int secondElement = dll.get(1);                                           // Get element at index 1
        std::cout << "The element at index 1 is: " << secondElement << std::endl; // Expected: 2
        int thirdElement = dll.get(2);                                            // Get element at index 2
        std::cout << "The element at index 2 is: " << thirdElement << std::endl;  // Expected: 3
    }
    catch (const std::out_of_range &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    // Test with an out-of-bounds index
    try
    {
        dll.get(5); // This should throw an exception
    }
    catch (const std::out_of_range &e)
    {
        std::cout << "Accessing index 5 gives an error: " << e.what() << std::endl; // Expected to catch an exception
    }

    // Create a list and add some elements
    DoublyLinkedList<int> original;
    original.push_back(1);
    original.push_back(2);
    original.push_back(3);

    // Use the copy constructor to create a copy of the list
    DoublyLinkedList<int> copy = original;

    // Modify the original list
    original.push_back(4);

    // Display the original list
    std::cout << "Original list: ";
    original.display_forward(); // Expected: 1 2 3 4

    // Display the copied list
    std::cout << "Copied list: ";
    copy.display_forward(); // Expected: 1 2 3

    return 0;
}

//  g++ -pg -fsanitize=address -g ./doublyLinkedlist_ptr.cpp -o dllptr -O3  && ./dllptr

// List in forward order : 2 1 3 4 List in backward order : 4 3 1 2 List after popping front and back : 1 3 Initial List : 2 1 1 3 3 4 List after deletions : 1 1 3 Adding elements at the end : 1 1 3 10 20 30

//     Adding elements at position 1 : 1 40 50 1 3 10 20 30

//     Adding elements at the beginning : 60 70 1 40 50 1 3 10 20 30

//     Adding more elements at the end : 60 70 1 40 50 1 3 10 20 30 80 90

//     Trying to add elements at an invalid position : Operation successful : false 60 70 1 40 50 1 3 10 20 30 80 90 4 5 true,
//                                                                            false The first element is : 1 The element at index 1 is : 2 The element at index 2 is : 3 Accessing index 5 gives an error : Index is out of bounds.Original list : 1 2 3 4 Copied list : 1 2 3

//                                                                                == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==
//                                                                            =
//                                                                                == 353 == ERROR : LeakSanitizer : detected memory leaks

//                                                                                                                      Indirect leak of 168 byte(s) in 3 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a33607fb in DoublyLinkedList<int> ::DoublyLinkedList(DoublyLinkedList<int> const &) doublyLinkedlist_ptr.cpp : 68
// # 4 0x55d5a3355fe4 in main doublyLinkedlist_ptr.cpp : 483
// # 5 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 168 byte(s) in 3 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a33550ba in DoublyLinkedList<int> ::addAll(std::initializer_list<int> const &) doublyLinkedlist_ptr.cpp : 154
// # 4 0x55d5a33550ba in main doublyLinkedlist_ptr.cpp : 385
// # 5 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 112 byte(s) in 2 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a33614e6 in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a33614e6 in DoublyLinkedList<int> ::addAll(int, std::initializer_list<int> const &) doublyLinkedlist_ptr.cpp : 196
// # 3 0x55d5a33551e9 in main doublyLinkedlist_ptr.cpp : 390
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 112 byte(s) in 2 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a335ff67 in DoublyLinkedList<int> ::clone() const doublyLinkedlist_ptr.cpp : 273
// # 4 0x55d5a3355ad0 in main doublyLinkedlist_ptr.cpp : 421
// # 5 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 112 byte(s) in 2 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a335751b in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a335751b in DoublyLinkedList<int> ::push_front(int const &) doublyLinkedlist_ptr.cpp : 94
// # 3 0x55d5a3360efe in DoublyLinkedList<int> ::addAll(int, std::initializer_list<int> const &) doublyLinkedlist_ptr.cpp : 173
// # 4 0x55d5a33552fb in main doublyLinkedlist_ptr.cpp : 395
// # 5 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a3355405 in DoublyLinkedList<int> ::addAll(std::initializer_list<int> const &) doublyLinkedlist_ptr.cpp : 154
// # 4 0x55d5a3355405 in main doublyLinkedlist_ptr.cpp : 400
// # 5 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a3355ca5 in main doublyLinkedlist_ptr.cpp : 438
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a3354e8a in main doublyLinkedlist_ptr.cpp : 369
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a33556a4 in main doublyLinkedlist_ptr.cpp : 412
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a3356019 in main doublyLinkedlist_ptr.cpp : 486
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a335566e in main doublyLinkedlist_ptr.cpp : 411
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a3355638 in main doublyLinkedlist_ptr.cpp : 410
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a3355ce8 in main doublyLinkedlist_ptr.cpp : 439
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a3355d2b in main doublyLinkedlist_ptr.cpp : 440
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a3355f56 in main doublyLinkedlist_ptr.cpp : 479
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a3355f0f in main doublyLinkedlist_ptr.cpp : 478
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a3355f9d in main doublyLinkedlist_ptr.cpp : 480
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a3358afb in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a3358afb in DoublyLinkedList<int> ::push_back(int const &) doublyLinkedlist_ptr.cpp : 76
// # 3 0x55d5a3355414 in DoublyLinkedList<int> ::addAll(std::initializer_list<int> const &) doublyLinkedlist_ptr.cpp : 154
// # 4 0x55d5a3355414 in main doublyLinkedlist_ptr.cpp : 400
// # 5 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a335751b in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a335751b in DoublyLinkedList<int> ::push_front(int const &) doublyLinkedlist_ptr.cpp : 94
// # 3 0x55d5a3354e1e in main doublyLinkedlist_ptr.cpp : 367
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     Indirect leak of 56 byte(s) in 1 object(s) allocated from :
// # 0 0x7f83c7381587 in operator new (unsigned long)../../../../ src / libsanitizer / asan / asan_new_delete.cc : 104
// # 1 0x55d5a335751b in std::shared_ptr<Node<int>> std::make_shared<Node<int>, int const &>(int const &) / usr / include / c ++ / 9 / ext / new_allocator.h : 114
// # 2 0x55d5a335751b in DoublyLinkedList<int> ::push_front(int const &) doublyLinkedlist_ptr.cpp : 94
// # 3 0x55d5a3354ce5 in main doublyLinkedlist_ptr.cpp : 349
// # 4 0x7f83c6cf8d8f(/ lib / x86_64 - linux - gnu / libc.so .6 + 0x29d8f)

//     SUMMARY : AddressSanitizer : 1512 byte(s) leaked in 27 allocation(s).