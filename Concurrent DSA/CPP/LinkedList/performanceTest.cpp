/*
 * Copyright (c) Cornell University.
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: I-Hsuan (Ethan) Huang
 * Email: ih246@cornell.edu
 */

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "LockFreeLinkedList.hpp"
#include "NonBlockingLinkedList.hpp"
#include "ConditionVariableLinkedList.hpp"
#include "NaiveLinkedList.hpp"
#include "FineGrainedLinkedList.hpp"

// Define a template function to get the name of the list type
template<typename T>
std::string getLinkedListTypeName();

// Specialize for each linked list type
template<>
std::string getLinkedListTypeName<NaiveLinkedList<int>>() {
    return "Naive Linked List";
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

template<typename LinkedListType>
void performanceTestLinkedList() {
    std::cout << "Testing : " << getLinkedListTypeName<LinkedListType>() << std::endl;
    for (int numThreads = 1; numThreads <= 128; numThreads*=2) {
        LinkedListType list;
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back([&list]() {
                for (int j = 0; j < 1000; ++j) {
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
        double opsPerMs = (numThreads * 3000) / elapsed.count();

        std::cout << "Threads: " << numThreads << ", Ops/ms: " << opsPerMs << ", Time: " << elapsed.count() << " ms" << std::endl;
    }
}

int main() {
    

    // Measure performance under concurrent access for each linked list implementation
    performanceTestLinkedList<NaiveLinkedList<int>>();
    performanceTestLinkedList<NonBlockingLinkedList<int>>();
    performanceTestLinkedList<ConditionVariableLinkedList<int>>();
    performanceTestLinkedList<LockFreeLinkedList<int>>();
    performanceTestLinkedList<FineGrainedLinkedList<int>>();

    return 0;
}


// g++ -pg -std=c++17 -o pT performanceTest.cpp -lpthread -O3
// valgrind --leak-check=full --show-leak-kinds=all ./pT