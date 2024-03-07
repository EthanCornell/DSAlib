#include "con_sema.hpp"

CountingSemaphore::CountingSemaphore(int initialCount) : count(initialCount) {}

void CountingSemaphore::wait()
{
    std::unique_lock<std::mutex> lock(mtx);
    while (count == 0)
    {
        cv.wait(lock);
    }
    --count;
}

void CountingSemaphore::signal()
{
    std::unique_lock<std::mutex> lock(mtx);
    ++count;
    cv.notify_one();
}
