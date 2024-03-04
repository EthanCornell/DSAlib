#include <chrono>
#include <forward_list>
#include <iostream>
#include <random>
#include "NaiveLinkedList_smartptr.hpp" // Ensure this path is correct

using namespace std;
using namespace std::chrono;

template <typename Func>
long long measureTimeMillis(Func f)
{
    auto start = high_resolution_clock::now();
    f();
    auto end = high_resolution_clock::now();
    return duration_cast<milliseconds>(end - start).count();
}

template <typename T>
void insertIntoForwardList(std::forward_list<T> &list, T value)
{
    list.push_front(value);
}

template <typename T>
bool searchForwardList(std::forward_list<T> &list, T value)
{
    for (const auto &item : list)
    {
        if (item == value)
        {
            return true;
        }
    }
    return false;
}

template <typename List, typename T>
void benchmarkList(size_t start, size_t end, size_t step)
{
    for (size_t size = start; size <= end; size *= step)
    {
        List list;

        // Measure insert
        auto insertTime = measureTimeMillis([&]
                                            {
            for (size_t i = 0; i < size; ++i) {
                if constexpr (is_same_v<List, std::forward_list<int>>) {
                    insertIntoForwardList(list, static_cast<int>(rand() % size));
                } else {
                    list.insert(static_cast<int>(rand() % size));
                }
            } });

        // Measure search
        auto searchTime = measureTimeMillis([&]
                                            {
            for (size_t i = 0; i < size; ++i) {
                if constexpr (is_same_v<List, std::forward_list<int>>) {
                    searchForwardList(list, static_cast<int>(rand() % size));
                } else {
                    list.search(static_cast<int>(rand() % size));
                }
            } });

        // Measure delete/remove
        auto deleteTime = measureTimeMillis([&]
                                            {
            for (size_t i = 0; i < size; ++i) {
                list.remove(static_cast<int>(rand() % size));
            } });

        cout << "Elements: " << size << ", Insert: " << insertTime << "ms, Search: " << searchTime << "ms, Delete: " << deleteTime << "ms\n";
    }
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    cout << "Benchmarking NaiveLinkedList_ptr\n";
    benchmarkList<NaiveLinkedList_ptr<int>, int>(10, 100000, 10);

    cout << "\nBenchmarking std::forward_list\n";
    benchmarkList<std::forward_list<int>, int>(10, 100000, 10);

    return 0;
}

//  g++ -std=c++17 -O2 benchmark.cpp -o benchmark
