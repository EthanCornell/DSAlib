#ifndef COUNTING_SEMAPHORE_HPP
#define COUNTING_SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>

class CountingSemaphore
{
public:
    explicit CountingSemaphore(int initialCount = 0);
    void wait();
    void signal();

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};

#endif // COUNTING_SEMAPHORE_HPP
