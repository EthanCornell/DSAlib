#ifndef NAIVE_LINKED_LIST_SMARTPTR_HPP
#define NAIVE_LINKED_LIST_SMARTPTR_HPP

#include <mutex>
#include <memory>


template<typename T>
class NaiveLinkedList_ptr {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;
        Node(T data) : data(data), next(nullptr) {}
    };

    std::unique_ptr<Node> head;
    std::mutex mtx;

public:
    NaiveLinkedList_ptr() : head(nullptr) {}

    void insert(T data) {
        std::lock_guard<std::mutex> lock(mtx);
        auto newNode = std::make_unique<Node>(data); // Use std::make_unique for safer memory allocation
        newNode->next = std::move(head);
        head = std::move(newNode);
    }

    bool search(T data) {
        std::lock_guard<std::mutex> lock(mtx);
        Node* current = head.get();
        while (current != nullptr) {
            if (current->data == data) return true;
            current = current->next.get();
        }
        return false;
    }

    bool remove(T data) {
        std::lock_guard<std::mutex> lock(mtx);
        Node* current = head.get();
        Node* prev = nullptr;
        while (current != nullptr) {
            if (current->data == data) {
                std::unique_ptr<Node>& toRemove = prev ? prev->next : head;
                std::unique_ptr<Node> temp = std::move(toRemove->next);
                toRemove = std::move(temp);
                return true;
            }
            prev = current;
            current = current->next.get();
        }
        return false;
    }
};



#endif // NAIVE_LINKED_LIST_SMARTPTR_HPP
