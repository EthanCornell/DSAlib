
//Benchmark and Test for FineGrainedLinkedList
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include "FineGrainedLinkedList.hpp"

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

// g++ -pg -std=c++17 -o FGLL FineGrainedLinkedList_test.cpp -lpthread -O3