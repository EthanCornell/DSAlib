#include <iostream>
#include <string>
#include <unordered_map>
#include <chrono>
#include <random>

#include "hashtable_cache.hpp"

void testSTLHashTable()
{
    std::unordered_map<int, std::string> umap;
    const int numItems = 10000000;                          // 10^7 items
    std::random_device rd;                                  // Obtain a random number from hardware
    std::mt19937 gen(rd());                                 // Seed the generator
    std::uniform_int_distribution<> distr(0, numItems - 1); // Define the range

    // Measure insertion time
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numItems; ++i)
    {
        int randomNumber = distr(gen);
        umap[randomNumber] = "Value for " + std::to_string(randomNumber);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> insertDuration = end - start;
    std::cout << "Insertion took " << insertDuration.count() << " milliseconds." << std::endl;

    // Measure search time
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numItems; ++i)
    {
        int randomNumber = distr(gen);
        auto search = umap.find(randomNumber);
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> searchDuration = end - start;
    std::cout << "Search took " << searchDuration.count() << " milliseconds." << std::endl;

    // Measure removal time
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numItems; ++i)
    {
        int randomNumber = distr(gen);
        umap.erase(randomNumber);
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> removeDuration = end - start;
    std::cout << "Removal took " << removeDuration.count() << " milliseconds." << std::endl;
}

void testCacheFriendlyHashTable()
{
    const int numItems = 10000000;
    CacheFriendlyHashTable<int, std::string> cHashTable(numItems); // Adjust size if necessary
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, numItems - 1);

    // Measure insertion time
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numItems; ++i)
    {
        int randomNumber = distr(gen);
        cHashTable.insert(i, "Value for " + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> insertDuration = end - start;
    std::cout << "Insertion took " << insertDuration.count() << " milliseconds." << std::endl;

    // Measure search time
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numItems; ++i)
    {
        int randomNumber = distr(gen);
        auto search = cHashTable.search(i);
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> searchDuration = end - start;
    std::cout << "Search took " << searchDuration.count() << " milliseconds." << std::endl;

    // Measure removal time
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numItems; ++i)
    {
        int randomNumber = distr(gen);
        cHashTable.remove(i);
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> removeDuration = end - start;
    std::cout << "Removal took " << removeDuration.count() << " milliseconds." << std::endl;
}

int main()
{
    std::cout << "Testing STL unordered_map with 10^7 items:" << std::endl;
    testSTLHashTable();

    std::cout << "\nTesting CacheFriendlyHashTable with 10^7 items:" << std::endl;
    testCacheFriendlyHashTable();

    return 0;
}

// g++ -pg -fsanitize=address -g -std=c++17 ./perf_test.cpp -o perf_t -O3  && ./perf_t