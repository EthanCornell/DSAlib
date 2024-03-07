#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class BoundedBuffer
{
private:
    int capacity;
    std::vector<T> buffer;
    std::mutex mutex;
    std::condition_variable not_full;
    std::condition_variable not_empty;

public:
    BoundedBuffer(int capacity) : capacity(capacity) {}

    void produce(T item)
    {
        std::unique_lock<std::mutex> lock(mutex);
        not_full.wait(lock, [this]()
                      { return buffer.size() < capacity; });
        buffer.push_back(item);
        std::cout << "Produced: " << item << std::endl;
        not_empty.notify_one();
    }

    T consume()
    {
        std::unique_lock<std::mutex> lock(mutex);
        not_empty.wait(lock, [this]()
                       { return !buffer.empty(); });
        T item = buffer.front();
        buffer.erase(buffer.begin());
        std::cout << "Consumed: " << item << std::endl;
        not_full.notify_one();
        return item;
    }
};

void producer(BoundedBuffer<int> &buffer)
{
    for (int i = 0; i < 20; ++i)
    {
        buffer.produce(i);
    }
}

void consumer(BoundedBuffer<int> &buffer)
{
    for (int i = 0; i < 20; ++i)
    {
        buffer.consume();
    }
}

int main()
{
    BoundedBuffer<int> buffer(10); // Buffer with capacity for 10 integers

    std::thread producerThread(producer, std::ref(buffer));
    std::thread consumerThread(consumer, std::ref(buffer));

    producerThread.join();
    consumerThread.join();

    return 0;
}
// g++ -pg -fsanitize=address -g ./BoundedBuffer_cp.cpp -o bbcp -O3  && ./bbcp