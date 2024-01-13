#include <atomic>
#include <memory>
#include <thread>
#include <iostream>

template<typename T>
class LockFreeQueue {
    struct Node {
        std::shared_ptr<T> data;
        std::atomic<std::shared_ptr<Node>> next;

        Node(T value) : data(std::make_shared<T>(value)), next(nullptr) {}
    };

    std::atomic<std::shared_ptr<Node>> head;
    std::atomic<std::shared_ptr<Node>> tail;

public:
    LockFreeQueue() : head(std::make_shared<Node>(T())), tail(head.load()) {}

    void enqueue(T value) {
        auto newNode = std::make_shared<Node>(value);
        while(true) {
            auto tailNode = tail.load();
            auto next = tailNode->next.load();
            if (tailNode == tail.load()) {
                if (!next) {
                    if (tailNode->next.compare_exchange_weak(next, newNode)) {
                        tail.compare_exchange_weak(tailNode, newNode);
                        return;
                    }
                } else {
                    tail.compare_exchange_weak(tailNode, next);
                }
            }
        }
    }

    bool dequeue(T& value) {
        while(true) {
            auto headNode = head.load();
            auto tailNode = tail.load();
            auto next = headNode->next.load();
            if (headNode == head.load()) {
                if (headNode == tailNode) {
                    if (!next) {
                        return false;
                    }
                    tail.compare_exchange_weak(tailNode, next);
                } else {
                    value = next->data;
                    if (head.compare_exchange_weak(headNode, next)) {
                        return true;
                    }
                }
            }
        }
    }
};

int main() {
    LockFreeQueue<int> queue;

    // Thread 1: Enqueue and Dequeue
    std::thread t1([&]() {
        for(int i = 0; i < 1000; ++i) {
            queue.enqueue(i);
            int value;
            if (queue.dequeue(value)) {
                // Handle dequeued value
                std::cout << "Dequeued: " << value << std::endl;
            }
        }
    });

    // Thread 2: Only Enqueue
    std::thread t2([&]() {
        for(int i = 0; i < 1000; ++i) {
            queue.enqueue(i);
        }
    });

    t1.join();
    t2.join();

    return 0;
}
