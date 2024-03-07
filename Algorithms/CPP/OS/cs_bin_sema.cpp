#include <iostream>
#include <thread>
#include <vector>
#include "bin_sema.hpp"
// Include the BinarySemaphore class definition here

void accessCriticalSection(BinarySemaphore &sem, int threadId)
{
    sem.wait(); // Enter critical section
    // Critical section: start
    std::cout << "Thread " << threadId << " is in the critical section." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
    std::cout << "Thread " << threadId << " is leaving the critical section." << std::endl;
    // Critical section: end
    sem.signal(); // Leave critical section
}

int main()
{
    const int numThreads = 5;
    BinarySemaphore sem;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i)
    {
        threads.push_back(std::thread(accessCriticalSection, std::ref(sem), i));
    }

    for (auto &t : threads)
    {
        t.join();
    }

    return 0;
}

// g++ -pg -fsanitize=address -g ./cs_bin_sema.cpp -o cs_bin_sema -O3  && ./cs_bin_sema