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

#include <atomic>
#include <vector>
#include <thread>
#include <cassert>

template <typename T>
class HazardPointer {
public:
    std::atomic<std::thread::id> owner;
    std::atomic<void*> pointer;

    HazardPointer() : owner(std::thread::id()), pointer(nullptr) {}
};

template <typename T>
class HPManager {
private:
    static const int MAX_HAZARD_POINTERS = 100;
    HazardPointer<T> hazardPointers[MAX_HAZARD_POINTERS];

public:
    HPManager() {}

    HazardPointer<T>* acquireHazardPointer();
    void releaseHazardPointer(HazardPointer<T>* hp);
    bool isPointerHazardous(void* ptr);
    // Additional methods for node retirement and reclamation would go here.
};

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


template<typename T>
HazardPointer<T>* HPManager<T>::acquireHazardPointer() {
    std::thread::id nullThreadId;
    const int maxRetries = 10; // Maximum number of retries
    for (int retry = 0; retry < maxRetries; ++retry) {
        for (int i = 0; i < MAX_HAZARD_POINTERS; ++i) {
            std::thread::id oldId = nullThreadId;
            if (hazardPointers[i].owner.compare_exchange_strong(oldId, std::this_thread::get_id(),
                                                                std::memory_order_acquire, std::memory_order_relaxed)) {
                return &hazardPointers[i];
            }
        }
        // Exponential backoff, No available hazard pointers
        std::this_thread::sleep_for(std::chrono::milliseconds(1 << retry)); // Sleep for 2^retry milliseconds
    }
    std::cerr << "Failed to acquire a hazard pointer: all hazard pointers are in use." << std::endl;
    return nullptr; // Consider throwing an exception or other error handling
}


template<typename T>
void HPManager<T>::releaseHazardPointer(HazardPointer<T>* hp) {
    hp->pointer.store(nullptr, std::memory_order_release);
    hp->owner.store(std::thread::id(), std::memory_order_release);
}

template<typename T>
bool HPManager<T>::isPointerHazardous(void* ptr) {
    for (int i = 0; i < MAX_HAZARD_POINTERS; ++i) {
        if (hazardPointers[i].pointer.load(std::memory_order_acquire) == ptr) {
            return true;
        }
    }
    return false;
}

#endif // HAZARD_POINTER_HPP
