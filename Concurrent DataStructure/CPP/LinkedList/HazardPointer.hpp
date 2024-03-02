/*
 * Copyright (c) Cornell University.
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: I-Hsuan (Ethan) Huang
 * Email: ih246@cornell.edu
 */

// HazardPointer.hpp

#ifndef HAZARD_POINTER_HPP
#define HAZARD_POINTER_HPP

#include <iostream>
#include <atomic>
#include <vector>
#include <thread>
#include <cassert>

template <typename T>
class HazardPointer
{
public:
    std::atomic<std::thread::id> owner;
    std::atomic<void *> pointer;

    HazardPointer() : owner(std::thread::id()), pointer(nullptr) {}
};

template <typename T>
class HPManager
{
private:
    static const int MAX_HAZARD_POINTERS = 100;
    HazardPointer<T> hazardPointers[MAX_HAZARD_POINTERS];
    std::vector<T *> retiredNodes; // List of retired nodes
    std::mutex retirementMutex;    // Protects access to retiredNodes
    std::atomic<bool> cleanupTaskShouldExit{false};
    std::thread cleanupTaskThread;

    void cleanupTask()
    {
        while (!cleanupTaskShouldExit.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust timing as needed
            reclaimRetiredNodes();
        }
        // Perform one final cleanup to ensure all nodes are processed
        reclaimRetiredNodes();
    }

public:
    HPManager();
    ~HPManager();

    HazardPointer<T> *acquireHazardPointer();
    void releaseHazardPointer(HazardPointer<T> *hp);
    bool isPointerHazardous(void *ptr);
    void retireNode(T *node);
    void reclaimRetiredNodes();
    void startBackgroundCleanupTask();
    void stopBackgroundCleanupTask();
};

// template <typename T>
// class HPManager {
// private:
//     static const int MAX_HAZARD_POINTERS = 100;
//     HazardPointer<T> hazardPointers[MAX_HAZARD_POINTERS];

// public:
//     HPManager() {}

//     HazardPointer<T>* acquireHazardPointer();
//     void releaseHazardPointer(HazardPointer<T>* hp);
//     bool isPointerHazardous(void* ptr);
//     // Additional methods for node retirement and reclamation would go here.
// };

// template<typename T>
// HazardPointer<T>* HPManager<T>::acquireHazardPointer() {
//     std::thread::id nullThreadId;
//     for (int i = 0; i < MAX_HAZARD_POINTERS; ++i) {
//         std::thread::id oldId = nullThreadId;
//         if (hazardPointers[i].owner.compare_exchange_strong(oldId, std::this_thread::get_id(),
//                                                             std::memory_order_acquire, std::memory_order_relaxed)) {
//             return &hazardPointers[i];
//         }
//     }
//     assert(false); // No available hazard pointers, should handle this case better in production code
//     return nullptr;
// }

template <typename T>
void HPManager<T>::retireNode(T *node)
{
    std::lock_guard<std::mutex> lock(retirementMutex);
    retiredNodes.push_back(node);
}

// Example logging utility
void log(const std::string &message)
{
    std::cout << message << std::endl;
}

template <typename T>
void HPManager<T>::reclaimRetiredNodes()
{
    std::lock_guard<std::mutex> lock(retirementMutex);
    log("Starting node reclamation");

    for (auto it = retiredNodes.begin(); it != retiredNodes.end();)
    {
        if (!isPointerHazardous(*it))
        {
            delete *it;                  // Safe to delete
            it = retiredNodes.erase(it); // Remove from the list
            log("Node reclaimed");
        }
        else
        {
            ++it;
        }
    }
}

template <typename T>
void HPManager<T>::startBackgroundCleanupTask()
{
    std::thread([this]()
                {
            while (!cleanupTaskShouldExit) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                reclaimRetiredNodes();
            } })
        .detach(); // Run the cleanup task in a detached thread
}

template <typename T>
void HPManager<T>::stopBackgroundCleanupTask()
{
    cleanupTaskShouldExit = true;
}

// HPManager<T>::HPManager() {
//     startBackgroundCleanupTask();
// }
template <typename T>
HPManager<T>::HPManager()
{
    cleanupTaskThread = std::thread(&HPManager::cleanupTask, this);
}

// HPManager<T>::~HPManager() {
//     stopBackgroundCleanupTask();
//     // Make sure to join the cleanup thread or ensure its completion before proceeding with destruction
//     // This ensures that all retired nodes are processed before the manager is destroyed
//     reclaimRetiredNodes(); // Final cleanup to ensure no nodes are left behind
// }
template <typename T>
HPManager<T>::~HPManager()
{
    cleanupTaskShouldExit.store(true);
    if (cleanupTaskThread.joinable())
    {
        cleanupTaskThread.join(); // Ensure the cleanup task completes before destruction
    }
}

template <typename T>
HazardPointer<T> *HPManager<T>::acquireHazardPointer()
{
    std::thread::id nullThreadId;
    const int maxRetries = 10; // Maximum number of retries
    for (int retry = 0; retry < maxRetries; ++retry)
    {
        for (int i = 0; i < MAX_HAZARD_POINTERS; ++i)
        {
            std::thread::id oldId = nullThreadId;
            if (hazardPointers[i].owner.compare_exchange_strong(oldId, std::this_thread::get_id(),
                                                                std::memory_order_acquire, std::memory_order_relaxed))
            {
                return &hazardPointers[i];
            }
        }
        // Exponential backoff, No available hazard pointers
        std::this_thread::sleep_for(std::chrono::milliseconds(1 << retry)); // Sleep for 2^retry milliseconds
    }
    std::cerr << "Failed to acquire a hazard pointer: all hazard pointers are in use." << std::endl;
    return nullptr; // Consider throwing an exception or other error handling
}

template <typename T>
void HPManager<T>::releaseHazardPointer(HazardPointer<T> *hp)
{
    hp->pointer.store(nullptr, std::memory_order_release);
    hp->owner.store(std::thread::id(), std::memory_order_release);
}

template <typename T>
bool HPManager<T>::isPointerHazardous(void *ptr)
{
    for (int i = 0; i < MAX_HAZARD_POINTERS; ++i)
    {
        if (hazardPointers[i].pointer.load(std::memory_order_acquire) == ptr)
        {
            return true;
        }
    }
    return false;
}

#endif // HAZARD_POINTER_HPP
