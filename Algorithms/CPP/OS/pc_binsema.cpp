#include <iostream>
#include <thread>
#include <queue>
#include "bin_sema.hpp"
// Include the BinarySemaphore class definition here

BinarySemaphore mutex(1), items(0);
std::queue<int> buffer;

void producer(int id)
{
    for (int i = 0; i < 5; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
        mutex.wait();                                                // Enter critical section
        buffer.push(i);
        std::cout << "Producer " << id << " produced " << i << std::endl;
        mutex.signal(); // Leave critical section
        items.signal(); // Signal consumer
    }
}

void consumer(int id)
{
    for (int i = 0; i < 5; ++i)
    {
        items.wait(); // Wait for an item to be produced
        mutex.wait(); // Enter critical section
        int item = buffer.front();
        buffer.pop();
        std::cout << "Consumer " << id << " consumed " << item << std::endl;
        mutex.signal(); // Leave critical section
    }
}

int main()
{
    std::thread p(producer, 0), c(consumer, 0);
    p.join();
    c.join();

    return 0;
}
