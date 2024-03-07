#include <mutex>
#include <condition_variable>
#include "bin_sema.hpp"
class BinarySemaphore
{
public:
    BinarySemaphore(int initial = 1) : count(initial) {}

    void wait()
    {
        std::unique_lock<std::mutex> lock(mtx);
        while (count == 0)
        {
            cv.wait(lock);
        }
        count = 0;
    }

    void signal()
    {
        std::unique_lock<std::mutex> lock(mtx);
        count = 1;
        cv.notify_one();
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};
