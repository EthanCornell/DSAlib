#include <iostream>
#include <thread>
#include "bin_sema.hpp"
// Include the BinarySemaphore class definition here
// In Order Execution
BinarySemaphore sem1(1), sem2(0), sem3(0);

void first()
{
    sem1.wait();
    std::cout << "First" << std::endl;
    sem2.signal();
}

void second()
{
    sem2.wait();
    std::cout << "Second" << std::endl;
    sem3.signal();
}

void third()
{
    sem3.wait();
    std::cout << "Third" << std::endl;
}

int main()
{
    std::thread t1(first), t2(second), t3(third);
    t1.join();
    t2.join();
    t3.join();

    return 0;
}

// g++ -pg -fsanitize=address -g ./BoundedBuffer_cp.cpp -o bbcp -O3  && ./bbcp