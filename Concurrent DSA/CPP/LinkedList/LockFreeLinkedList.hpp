// LockFreeLinkedList.hpp
#ifndef LOCK_FREE_LINKED_LIST_HPP
#define LOCK_FREE_LINKED_LIST_HPP

#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include "HazardPointer.hpp"

template <typename T>
class LockFreeLinkedList {
private:
    struct Node {
        T data;
        std::atomic<Node*> next;
        Node(T data) : data(data), next(nullptr) {}
    };

    std::atomic<Node*> head;
    HPManager<Node> hpManager; // Assuming HPManager is templated on the node type

public:
    LockFreeLinkedList() : head(nullptr) {}

    ~LockFreeLinkedList() {
        Node* current = head.load();
        while (current != nullptr) {
            Node* next = current->next.load();
            delete current;
            current = next;
        }
    }

    void insert(T data) {
        Node* newNode = new Node(data);
        Node* expectedHead = head.load(std::memory_order_relaxed);
        newNode->next = expectedHead; // Initialize newNode->next
        // Try to set the new node as the new head
        while (!head.compare_exchange_weak(expectedHead, newNode,
                                           std::memory_order_release,
                                           std::memory_order_relaxed)) {
            // CAS failed, expectedHead was updated with the current head,
            // Update newNode->next and try again
            newNode->next = expectedHead;
        }
    }


    bool search(T data) {
        HazardPointer<Node>* hp = hpManager.acquireHazardPointer(); // Assuming HPManager is now correctly handling Node types
        Node* current = head.load(std::memory_order_acquire);
        while (current != nullptr) {
            hp->pointer.store(current, std::memory_order_release); // Correctly typed now
            if (current->data == data) {
                hpManager.releaseHazardPointer(hp);
                return true; // Data found
            }
            current = current->next.load(std::memory_order_acquire);
        }
        hpManager.releaseHazardPointer(hp);
        return false; // Data not found
    }

    // Safe remove using hazard pointers
    bool remove(T data)
    {
        std::vector<Node *> retiredNodes; // For later safe deletion
        HazardPointer<Node> *hp = hpManager.acquireHazardPointer();
        Node *prev = nullptr;
        Node *current = head.load(std::memory_order_acquire);
        bool deleted = false; // Flag to indicate deletion

        while (current != nullptr)
        {
            Node *next = current->next.load(std::memory_order_acquire);
            hp->pointer.store(current, std::memory_order_release); // Protect with hazard pointer

            if (current->data == data)
            {
                // Attempt to unlink the node safely
                if (prev == nullptr)
                {
                    if (head.compare_exchange_strong(current, next, std::memory_order_release))
                    {
                        retiredNodes.push_back(current); // Schedule for safe deletion
                        deleted = true;                  // Indicate successful deletion
                        break;                           // Exit loop after deletion
                    }
                }
                else
                {
                    if (prev->next.compare_exchange_strong(current, next, std::memory_order_release))
                    {
                        retiredNodes.push_back(current); // Schedule for safe deletion
                        deleted = true;                  // Indicate successful deletion
                        break;                           // Exit loop after deletion
                    }
                }
            }
            prev = current; // Move prev forward
            current = next; // Move current forward
        }

        // Safe deletion of retired nodes, ensuring they are no longer accessed
        for (Node *node : retiredNodes)
        {
            if (!hpManager.isPointerHazardous(node))
            {
                delete node; // Now safe to delete
            }
            // If still hazardous, consider adding back to a global retire list for later reclamation
        }

        hpManager.releaseHazardPointer(hp);
        return deleted; // Return true if a node was removed
    }
};

#endif // LOCK_FREE_LINKED_LIST_HPP
