#ifndef NAIVE_LINKED_LIST_HPP
#define NAIVE_LINKED_LIST_HPP

#include <mutex>
#include <iostream>

template<typename T>
class NaiveLinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(T data) : data(data), next(nullptr) {}
    };

    Node* head;
    std::mutex mtx;

public:
    NaiveLinkedList() : head(nullptr) {}

    ~NaiveLinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    void insert(T data) {
        std::lock_guard<std::mutex> lock(mtx);
        Node* newNode = new Node(data);
        newNode->next = head;
        head = newNode;
    }

    bool search(T data) {
        std::lock_guard<std::mutex> lock(mtx);
        Node* current = head;
        while (current != nullptr) {
            if (current->data == data) return true;
            current = current->next;
        }
        return false;
    }

    bool remove(T data) {
        std::lock_guard<std::mutex> lock(mtx);
        Node *current = head, *prev = nullptr;
        while (current != nullptr) {
            if (current->data == data) {
                if (prev == nullptr) {
                    head = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }
};

#endif // NAIVE_LINKED_LIST_HPP
