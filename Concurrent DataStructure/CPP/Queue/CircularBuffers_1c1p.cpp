#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>

class CircularBuffer
{
public:
    CircularBuffer(size_t size) : buf_(std::vector<int>(size)), max_size_(size) {}

    void put(int item)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        // Wait until space is available
        not_full_.wait(lock, [this]()
                       { return count_ < max_size_; });
        // Insert item into buffer
        buf_[tail_] = item;
        tail_ = (tail_ + 1) % max_size_;
        ++count_;
        // Signal to consumer that data is available
        not_empty_.notify_one();
    }

    int get()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        // Wait until data is available
        not_empty_.wait(lock, [this]()
                        { return count_ > 0; });
        // Retrieve item from buffer
        int item = buf_[head_];
        head_ = (head_ + 1) % max_size_;
        --count_;
        // Signal to producer that space is available
        not_full_.notify_one();
        return item;
    }

private:
    std::vector<int> buf_;
    size_t head_ = 0;
    size_t tail_ = 0;
    const size_t max_size_;
    size_t count_ = 0;
    std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
};

// Example producer function
void producer(CircularBuffer &buffer)
{
    for (int i = 0; i < 10; ++i)
    {
        buffer.put(i);
        std::cout << "Produced: " << i << std::endl;
    }
}

// Example consumer function
void consumer(CircularBuffer &buffer)
{
    for (int i = 0; i < 10; ++i)
    {
        int item = buffer.get();
        std::cout << "Consumed: " << item << std::endl;
    }
}

int main()
{
    CircularBuffer buffer(5); // Create a circular buffer of size 5

    std::thread producerThread(producer, std::ref(buffer));
    std::thread consumerThread(consumer, std::ref(buffer));

    producerThread.join();
    consumerThread.join();

    return 0;
}

// g++ -pg -fsanitize=address -g ./CircularBuffer_1c1p.cpp -o cb1c1p -O3  && ./cb1c1p