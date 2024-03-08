#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

class ReadersWriters
{
public:
    ReadersWriters() : readers_count(0), writer_active(false) {}

    void startRead()
    {
        std::unique_lock<std::mutex> lock(mutex);
        readers_condition.wait(lock, [this]()
                               { return !writer_active; });
        readers_count++;
    }

    void endRead()
    {
        std::unique_lock<std::mutex> lock(mutex);
        readers_count--;
        if (readers_count == 0)
        {
            writer_condition.notify_one();
        }
    }

    void startWrite()
    {
        std::unique_lock<std::mutex> lock(mutex);
        writer_condition.wait(lock, [this]()
                              { return !writer_active && readers_count == 0; });
        writer_active = true;
    }

    void endWrite()
    {
        std::unique_lock<std::mutex> lock(mutex);
        writer_active = false;
        readers_condition.notify_all();
        writer_condition.notify_one();
    }

private:
    int readers_count;
    bool writer_active;
    std::mutex mutex;
    std::condition_variable readers_condition;
    std::condition_variable writer_condition;
};

void reader(ReadersWriters &sharedResource, int id)
{
    sharedResource.startRead();
    std::cout << "Reader " << id << " starts reading." << std::endl;
    // Simulate reading time
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Reader " << id << " finished reading." << std::endl;
    sharedResource.endRead();
}

void writer(ReadersWriters &sharedResource, int id)
{
    sharedResource.startWrite();
    std::cout << "Writer " << id << " starts writing." << std::endl;
    // Simulate writing time
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Writer " << id << " finished writing." << std::endl;
    sharedResource.endWrite();
}

int main()
{
    ReadersWriters sharedResource;

    std::vector<std::thread> threads;

    // Create a mix of readers and writers for demonstration
    for (int i = 0; i < 5; ++i)
    {
        threads.push_back(std::thread(reader, std::ref(sharedResource), i));
    }

    for (int i = 0; i < 2; ++i)
    {
        threads.push_back(std::thread(writer, std::ref(sharedResource), i));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    return 0;
}

// g++ -pg -fsanitize=address -g -std=c++11 -pthread readers_writers.cpp -o readers_writers -O3