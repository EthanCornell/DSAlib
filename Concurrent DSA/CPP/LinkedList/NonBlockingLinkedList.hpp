#ifndef NON_BLOCKING_LINKED_LIST_HPP
#define NON_BLOCKING_LINKED_LIST_HPP

#include <atomic>
#include <memory>
#include <iostream>

template<typename T>
class Node {
public:
    T key;
    std::shared_ptr<Node<T>> next;
    std::atomic<bool> marked;

    Node(T key) : key(key), next(nullptr), marked(false) {}
};

template<typename T>
class NonBlockingLinkedList {
private:
    std::shared_ptr<Node<T>> head;
    std::shared_ptr<Node<T>> tail;

public:
    NonBlockingLinkedList() {
        head = std::make_shared<Node<T>>(T{});
        tail = std::make_shared<Node<T>>(T{});
        std::atomic_store(&head->next, tail);
    }

    bool insert(T key) {
        while (true) {
            auto [left_node, right_node] = searchForNodes(key);
            if (right_node != tail && right_node->key == key)
                return false;
            auto new_node = std::make_shared<Node<T>>(key);
            new_node->next = right_node;
            if (std::atomic_compare_exchange_weak(&left_node->next, &right_node, new_node))
                return true;
        }
    }

    bool remove(T key) {
        while (true) {
            auto [left_node, right_node] = searchForNodes(key);
            if (right_node == tail || right_node->key != key)
                return false;

            auto right_node_next = std::atomic_load(&right_node->next);
            if (std::atomic_compare_exchange_weak(&right_node->next, &right_node_next, right_node_next)) {
                if (std::atomic_compare_exchange_weak(&left_node->next, &right_node, right_node_next))
                    return true;
            }
        }
    }

    bool search(T key) {
        auto [left_node, right_node] = searchForNodes(key);
        return !(right_node == tail || right_node->key != key);
    }

private:
    std::pair<std::shared_ptr<Node<T>>, std::shared_ptr<Node<T>>> searchForNodes(T search_key) {
        std::shared_ptr<Node<T>> left_node = head, right_node;

        do {
            std::shared_ptr<Node<T>> t = head, t_next;
            do {
                if (!is_marked(std::atomic_load(&t->next))) {
                    left_node = t;
                }
                t = get_unmarked(std::atomic_load(&t->next));
                if (t == tail) break;
                t_next = std::atomic_load(&t->next);
            } while (is_marked(t_next) || (t->key < search_key && t != tail));

            right_node = t;

            if (std::atomic_load(&left_node->next) == right_node) {
                if (right_node != tail && is_marked(std::atomic_load(&right_node->next)))
                    continue;
                else
                    break;
            }

        } while (true);

        return {left_node, right_node};
    }

    bool is_marked(const std::shared_ptr<Node<T>>& node) {
        return node->marked.load();
    }

    std::shared_ptr<Node<T>> get_unmarked(const std::shared_ptr<Node<T>>& node) {
        return node;
    }
};

#endif // NON_BLOCKING_LINKED_LIST_HPP
