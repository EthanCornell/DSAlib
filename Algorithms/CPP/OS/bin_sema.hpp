#ifndef BINARY_SEMAPHORE_HPP
#define BINARY_SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>

class BinarySemaphore
{
public:
    explicit BinarySemaphore(int initial = 1);

    void wait();
    void signal();

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};

#endif // BINARY_SEMAPHORE_HPP
