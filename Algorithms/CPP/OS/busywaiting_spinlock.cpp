#include <iostream>
#include <thread>
#include <atomic>
#include <vector>

// busy waiting with shared memory or resource in C++, one common approach is to use atomic variables and spinlocks.
// A spinlock is a lock which causes a thread trying to acquire it to simply wait in a loop ("spin") while repeatedly checking if the lock is available.
// Since the thread remains active but isn't performing a useful task, it's called busy waiting.

class Spinlock
{
private:
    std::atomic<bool> lockFlag = {false};

public:
    void lock()
    {
        bool expected = false;
        // Attempt to set lockFlag to true if it is currently false (expected).
        // This loop will continue until the lock is successfully acquired.
        while (!lockFlag.compare_exchange_weak(expected, true, std::memory_order_acquire))
        {
            expected = false; // Reset expected since compare_exchange_weak modifies it.
        }
    }

    void unlock()
    {
        lockFlag.store(false, std::memory_order_release);
    }
};

// Shared resource
int counter = 0;
Spinlock spinlock;

void incrementCounter(int numIncrements)
{
    for (int i = 0; i < numIncrements; ++i)
    {
        spinlock.lock(); // Busy waiting occurs here if another thread holds the lock.
        ++counter;       // Critical section: modifying shared resource.
        spinlock.unlock();
    }
}

int main()
{
    const int numThreads = 4;
    const int incrementsPerThread = 10000;

    std::vector<std::thread> threads;

    // Create and start threads
    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(incrementCounter, incrementsPerThread);
    }

    // Join threads
    for (auto &thread : threads)
    {
        thread.join();
    }

    std::cout << "Final counter value: " << counter << std::endl;

    return 0;
}

// g++ -pg -fsanitize=address -g -std=c++17 ./busywaiting_spinlock.cpp -o bwspinlock -O3  && ./bwspinlock