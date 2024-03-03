#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <fstream>
#include "LockFreeLinkedList.hpp"
#include "NonBlockingLinkedList.hpp"
#include "ConditionVariableLinkedList.hpp"
#include "NaiveLinkedList.hpp"
#include "FineGrainedLinkedList.hpp"
#include "NaiveLinkedList_smartptr.hpp"

// Define a template function to get the name of the list type
template<typename T>
std::string getLinkedListTypeName();

// Specialize for each linked list type
template<>
std::string getLinkedListTypeName<NaiveLinkedList<int>>() {
    return "Naive Linked List";
}

template<>
std::string getLinkedListTypeName<NaiveLinkedList_ptr<int>>() {
    return "Naive Linked List With Smart Pointer";
}

template<>
std::string getLinkedListTypeName<NonBlockingLinkedList<int>>() {
    return "Non-Blocking Linked List";
}

template<>
std::string getLinkedListTypeName<ConditionVariableLinkedList<int>>() {
    return "Condition Variable Linked List";
}

template<>
std::string getLinkedListTypeName<LockFreeLinkedList<int>>() {
    return "Lock-Free Linked List";
}

template<>
std::string getLinkedListTypeName<FineGrainedLinkedList<int>>() {
    return "Fine Grained Linked List";
}

// Function to measure memory usage (implementation depends on your platform)
size_t getMemoryUsage()
{
    long pages = sysconf(_SC_PAGESIZE); // Get the system's page size
    std::ifstream statm("/proc/self/statm");
    size_t size;
    statm >> size; // The first field in /proc/self/statm is total program size
    statm.close();
    return size * pages; // Convert from pages to bytes
}

template<typename LinkedListType>
void threadWork(LinkedListType &list)
{
    // Example workload for each thread
    for (int i = 0; i < 1000; ++i)
    {
        int value =  rand()%1000;;
        list.insert(value);
        list.search(value);
        list.remove(value);
    }
}

template<typename LinkedListType>
void memoryTestLinkedList() {
    std::cout << "Memory Testing: " << getLinkedListTypeName<LinkedListType>() << std::endl;
    for (int numThreads = 1; numThreads <= 128; numThreads *= 2) {
        LinkedListType list;

        auto startMemUsage = getMemoryUsage();

        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(threadWork<LinkedListType>, std::ref(list));
        }

        for (auto& t : threads) {
            t.join();
        }

        auto endMemUsage = getMemoryUsage();

        // Convert memory usage from bytes to megabytes
        double startMemUsageMB = startMemUsage / (1024.0 * 1024.0);
        double endMemUsageMB = endMemUsage / (1024.0 * 1024.0);
        double deltaMemUsageMB = (endMemUsage - startMemUsage) / (1024.0 * 1024.0);

        std::cout << "Threads: " << numThreads
                  << ", Start Memory: " << startMemUsageMB << " MB"
                  << ", End Memory: " << endMemUsageMB << " MB"
                  << ", Delta Memory: " << deltaMemUsageMB << " MB" << std::endl;
    }
}

int main() {
    srand(time(NULL)); // Seed the random number generator

    // Measure memory usage for each linked list implementation
    memoryTestLinkedList<NaiveLinkedList<int>>();
    memoryTestLinkedList<NaiveLinkedList_ptr<int>>();
    memoryTestLinkedList<NonBlockingLinkedList<int>>();
    memoryTestLinkedList<ConditionVariableLinkedList<int>>();
    memoryTestLinkedList<LockFreeLinkedList<int>>();
    memoryTestLinkedList<FineGrainedLinkedList<int>>();

    return 0;
}

// g++ -pg -std=c++17 -o memoryTest memoryTest.cpp -lpthread -O3  && ./memoryTest
// valgrind --leak-check=full --show-leak-kinds=all ./memoryTest
