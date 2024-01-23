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
class CircularDoublyLinkedList {
private:
    std::unique_ptr<Node<T>> head;
    Node<T>* tail;

public:
    CircularDoublyLinkedList() : head(nullptr), tail(nullptr) {}
    ~CircularDoublyLinkedList() {
        if (head) {
            tail->next = nullptr; // Break the circular link
        }
        while (head) {
            head = std::move(head->next);
        }
    }

    void push_back(T value);
    void push_front(T value);
    void pop_back();
    void pop_front();
    void insert(int index, T value);
    void delete_node(T value);
    void display();
    // Other member functions can be added as needed
};




// push_front: Add a new node at the beginning of the list.
template<typename T>
void CircularDoublyLinkedList<T>::push_front(T value) {
    // Implement push_front
}


// push_back, the time complexity is O(1) as we are adding elements at the end.
// push_back: Add a new node at the end of the list.
template<typename T>
void CircularDoublyLinkedList<T>::push_back(T value) {
    // Implement push_back
}




// pop_back: Remove the last node of the list.
template<typename T>
void CircularDoublyLinkedList<T>::pop_back() {
    // Implement pop_back
}


// pop_front: Remove the first node of the list.
template<typename T>
void CircularDoublyLinkedList<T>::pop_front() {
    // Implement pop_front
}



// insert: Insert a new node at a given index.
template<typename T>
void CircularDoublyLinkedList<T>::insert(int index, T value) {
    if (index == 0) {
        push_front(value);
        return;
    }
    Node<T>* current = head.get();
    for (int i = 0; current != nullptr && i < index - 1; i++) {
        current = current->next.get();
    }
    if (!current) return; // Index out of bounds

    std::unique_ptr<Node<T>> newNode = std::make_unique<Node<T>>(value);
    newNode->next = std::move(current->next);
    newNode->prev = current;
    newNode->next->prev = newNode.get();
    current->next = std::move(newNode);
    if (current == tail) {
        tail = tail->next.get();
    }
}

// delete_node: Delete a node with a given value.
template<typename T>
void CircularDoublyLinkedList<T>::delete_node(T value) {
    if (!head) return; // Empty list

    if (head->data == value) {
        pop_front();
        return;
    }

    Node<T>* current = head->next.get();
    while (current != head.get() && current->data != value) {
        current = current->next.get();
    }

    if (current == head.get()) return; // Value not found

    current->prev->next = std::move(current->next);
    current->next->prev = current->prev;
    if (current == tail) {
        tail = current->prev;
    }
}


// Time Complexity
// push_front, pop_back, pop_front: O(1)
// insert: O(n) in the worst case (when inserting at the end)
// delete_node: O(n) in the worst case (when the node is at the end or not present)
// Space Complexity
// O(n) for all operations, where n is the number of elements in the list.

template<typename T>
void CircularDoublyLinkedList<T>::display() {
    if (!head) {
        std::cout << "List is empty" << std::endl;
        return;
    }
    Node<T>* current = head.get();
    do {
        std::cout << current->data << " ";
        current = current->next.get();
    } while (current != head.get());
    std::cout << std::endl;
}



int main() {
    CircularDoublyLinkedList<int> list;

    // Test cases
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    std::cout << "List contents: ";
    list.display(); // Expected: 1 2 3

    // Test push_front and push_back
    list.push_front(1);
    list.push_back(2);
    list.push_front(0);
    list.display(); // Expected: 0 1 2

    // Test pop_back and pop_front
    list.pop_back();
    list.pop_front();
    list.display(); // Expected: 1

    // Test insert
    list.insert(1, 3);
    list.display(); // Expected: 1 3

    // Test delete_node
    list.delete_node(1);
    list.display(); // Expected: 3

    // Add more test cases as needed

    return 0;
}
