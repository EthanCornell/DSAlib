#include <iostream>
#include <memory> // For std::unique_ptr
#include <mutex>  // For std::mutex and std::lock_guard

template<typename T>
class FineGrainedLinkedList {
public:
    struct Node {
        T value;
        std::unique_ptr<Node> next;
        std::mutex mutex;

        Node(T val) : value(val), next(nullptr) {}
    };

    FineGrainedLinkedList() : head(nullptr) {}

    // Function to insert a new value into the list
    void insert(T value) {
        std::unique_ptr<Node> newNode = std::make_unique<Node>(value);

        // Lock head mutation
        std::lock_guard<std::mutex> lockHead(headMutex);

        if (!head || head->value > value) { // Insertion at the beginning
            newNode->next = std::move(head);
            head = std::move(newNode);
            return;
        }

        // Locking nodes for thread-safe insertion
        Node* prev = head.get();
        prev->mutex.lock();
        Node* current = prev->next.get();

        while (current) {
            current->mutex.lock();
            if (current->value > value) {
                break;
            }
            prev->mutex.unlock(); // Unlock the previous node after locking the current one
            prev = current;
            current = current->next.get();
        }

        newNode->next = std::move(prev->next);
        prev->next = std::move(newNode);
        prev->mutex.unlock();
        if (current) current->mutex.unlock();
    }

    // Function to search for a value in the list
    bool search(T value) {
        std::lock_guard<std::mutex> lockHead(headMutex);

        Node* current = head.get();
        while (current) {
            std::lock_guard<std::mutex> lockCurrent(current->mutex);
            if (current->value == value) {
                return true;
            }
            current = current->next.get();
        }
        return false;
    }

    // Function to remove a value from the list
    bool remove(T value) {
        std::lock_guard<std::mutex> lockHead(headMutex);

        if (!head) return false;

        if (head->value == value) { // Removal of the head
            head = std::move(head->next);
            return true;
        }

        Node* prev = head.get();
        prev->mutex.lock();
        Node* current = prev->next.get();

        while (current) {
            current->mutex.lock();
            if (current->value == value) {
                prev->next = std::move(current->next);
                prev->mutex.unlock();
                current->mutex.unlock(); // Unlock before deletion to avoid deadlock
                return true;
            }
            prev->mutex.unlock(); // Move the locks along the list
            prev = current;
            current = current->next.get();
        }

        if (prev) prev->mutex.unlock();
        if (current) current->mutex.unlock();
        return false;
    }

private:
    std::unique_ptr<Node> head;
    std::mutex headMutex; // Protects modifications to the head pointer
};



//Benchmark and Test
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <cassert>


void correctnessTest() {
    FineGrainedLinkedList<int> list;

    // Insert elements
    list.insert(1);
    list.insert(2);
    list.insert(3);

    // Search elements
    assert(list.search(1) == true);
    assert(list.search(2) == true);
    assert(list.search(3) == true);
    assert(list.search(4) == false); // Should not be found

    // Remove elements
    assert(list.remove(2) == true);
    assert(list.search(2) == false); // Should not be found after removal
    assert(list.remove(4) == false); // Should not be found for removal

    std::cout << "Correctness test passed." << std::endl;
}

void threadSafeTest() {
    FineGrainedLinkedList<int> list;
    const int numThreads = 8;      // Example: Adjust based on your testing needs
    const int numOperations = 100; // Operations per thread

    auto worker = [&list](int threadId) {
        for (int i = 0; i < numOperations; ++i) {
            // Even threads insert and search, odd threads search and remove
            if (threadId % 2 == 0) {
                list.insert(i);
                list.search(i);
            } else {
                list.search(i);
                list.remove(i);
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Thread safety test completed." << std::endl;
}

void performanceTest() {
    for (int numThreads = 1; numThreads <= 128; numThreads++) {
        FineGrainedLinkedList<int> list;
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back([&list]() {
                for (int j = 0; j < 1000; ++j) { // Each thread performs 1000 operations
                    list.insert(j);
                    list.search(j);
                    list.remove(j);
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        double opsPerMs = (numThreads * 3000) / elapsed.count(); // 3000 operations: insert, search, remove

        std::cout << "Threads: " << numThreads << ", Ops/ms: " << opsPerMs << ", Time: " << elapsed.count() << " ms" << std::endl;
    }
}

int main() {
    correctnessTest(); // Validate basic functionality
    threadSafeTest();  // Ensure thread safety
    performanceTest(); // Measure performance under concurrent access

    return 0;
}

// g++ -pg -std=c++17 -o FGLL FineGrainedLinkedList.cpp -lpthread -O3