#include <omp.h>
#include <chrono>
#include <iostream>
#include <vector>
#include "LockFreeLinkedList_para.hpp" 
#include "LockFreeLinkedList.hpp"


void benchmark(size_t numThreads) {
    LockFreeLinkedList<int> list;
    const int numElements = 10000; // Number of operations per thread

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back([&list, i, numElements]() {
            for (int n = 0; n < numElements; ++n) {
                int value = i * numElements + n;
                list.insert(value);
                list.search(value);
                list.remove(value);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    double ms = duration.count();
    double opsPerMs = (numThreads * numElements * 3) / ms; // Multiply by 3 because we insert, search, and remove

    std::cout << "Threads: " << numThreads << ", Time: " << ms << " ms, Ops/ms: " << opsPerMs << std::endl;
}

void benchmarkOpenMP(size_t numThreads, size_t numElements = 10000) {
    LockFreeLinkedList_para<int> list;
    std::vector<int> data(numElements);
    std::vector<int> searchValues(numElements);
    std::vector<int> itemsToRemove(numElements);

    

    omp_set_num_threads(numThreads);

    auto start = std::chrono::high_resolution_clock::now();

    // Prepare data for operations
    for (size_t i = 0; i < numElements; ++i) {
        data[i] =  i * numElements + i;
        searchValues[i] = i * numElements + i;
        itemsToRemove[i] = i * numElements + i;
    }
    // Parallel insert
    list.parallelInsert(list, data);
    // Parallel search
    list.parallelSearch(list,searchValues);
    // Parallel remove
    list.parallelRemove(list, itemsToRemove);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    double ms = duration.count();
    double opsPerMs = (numThreads * numElements * 3) / ms; // Multiply by 3 because we insert, search, and remove

    std::cout << "Threads: " << numThreads << ", Time: " << ms << " ms, Ops/ms: " << opsPerMs << std::endl;
}

int main() {
    
    std::cout<< "Parallel LockFree Linked List \n" << std::endl;
    
    for (size_t numThreads = 1; numThreads <= 128; numThreads *= 2) { // Adjust the number of threads as needed
        
        benchmarkOpenMP(numThreads);
    }
    std::cout<< "\n" << std::endl;

    std::cout<< "LockFree Linked List \n" << std::endl;
    
    for (size_t numThreads = 1; numThreads <= 128; numThreads *= 2) { // Adjust the number of threads as needed
        benchmark(numThreads); 
    }

    
    return 0;
}


// g++ -pg -fopenmp -std=c++17 -o para paraTest.cpp -lpthread -O3 && ./para