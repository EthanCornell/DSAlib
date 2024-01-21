 /*
 * Author: I-Hsuan (Ethan) Huang
 * Email: ih246@cornell.edu
 */

//lock-free FIFO queue

#include <atomic>
#include <iostream>
#include <thread> // For Main test
#include <vector> // For Main test
#include <cstdint> // For uintptr_t
#include <cassert> // For assert


// Data type to store in the queue.
using data_type = int; // Defining data type for queue elements
const data_type dummy_val = -1; // A special value to represent dummy nodes in the queue

struct node_t; // Forward declaration of node_t

struct pointer_t {
    uintptr_t combined; // Combined field to store pointer and tag

    pointer_t(node_t* ptr = nullptr, unsigned int tag = 0) {
        set(ptr, tag); // Constructor to set pointer and tag
    }

    node_t* getPtr() const {
        // Extract and return the pointer part from the combined field
        return reinterpret_cast<node_t*>(combined & ~static_cast<uintptr_t>(0xFF));
    }

    unsigned int getTag() const {
        // Extract and return the tag part from the combined field
        return static_cast<unsigned int>(combined & 0xFF);
    }

    void set(node_t* ptr, unsigned int tag) {
        // Combine ptr and tag into the combined field
        assert(tag < 256); // Ensure the tag fits into 8 bits
        uintptr_t ptrVal = reinterpret_cast<uintptr_t>(ptr);
        combined = ptrVal | tag;
    }

    bool operator==(const pointer_t& other) const {
        // Overload == operator for pointer_t comparison
        return combined == other.combined;
    }

    bool operator!=(const pointer_t& other) const {
        // Overload != operator for pointer_t comparison
        return combined != other.combined;
    }
};

struct node_t {
    data_type value; // Stores the value of the node
    std::atomic<pointer_t> next; // Atomic pointer to the next node
    std::atomic<pointer_t> prev; // Atomic pointer to the previous node
};

struct queue_t {
    std::atomic<pointer_t> head; // Atomic pointer to the head of the queue
    std::atomic<pointer_t> tail; // Atomic pointer to the tail of the queue
};

node_t* new_node(data_type val = 0) {
    node_t* node = new node_t; // Dynamically allocate a new node
    node->value = val; // Set the node's value
    node->next.store({nullptr, 0}); // Initialize next pointer
    node->prev.store({nullptr, 0}); // Initialize prev pointer
    return node; // Return the new node
}

bool CAS(std::atomic<pointer_t>& obj, pointer_t& expected, pointer_t desired) {
    // Atomic compare-and-swap operation
    return std::atomic_compare_exchange_strong(&obj, &expected, desired);
}

void enqueue(queue_t* q, data_type val) {
    node_t* nd = new_node(val); // Create a new node with the value
    pointer_t tail;

    while (true) {
        tail = q->tail.load(); // Load the current tail of the queue

        pointer_t newTailNext;
        newTailNext.set(nd, tail.getTag() + 1); // Prepare the new tail's next pointer
        nd->next.store(newTailNext); // Store the new next pointer

        if (CAS(q->tail, tail, {nd, tail.getTag() + 1})) {
            // If the CAS is successful
            node_t* tailNode = tail.getPtr();
            if (tailNode != nullptr) {
                pointer_t newTailPrev;
                newTailPrev.set(nd, tail.getTag());
                tailNode->prev.store(newTailPrev); // Update the previous pointer of the old tail
            }
            break; // Break the loop on successful CAS
        }
    }
}


// FixList function: Function to fix the queue in case of inconsistencies
void fixList(queue_t* q, pointer_t tail, pointer_t head) {
    pointer_t curNode, curNodeNext, nextNodePrev;

    curNode = tail; // Start from the tail.

    while ((head == q->head.load()) && (curNode.getPtr() != head.getPtr())) {
        // Read the next pointer of the current node.
        curNodeNext = curNode.getPtr()->next.load();


        if (curNodeNext.getTag() != curNode.getTag()) {
            // If the tags don't match, it indicates a potential inconsistency.
            return; // Exit the function to avoid further disruption.
        }

        nextNodePrev = curNodeNext.getPtr()->prev.load(); // Read the prev pointer of the next node.

        if (nextNodePrev.getPtr() != curNode.getPtr() || nextNodePrev.getTag() != curNode.getTag() - 1) {
            // If the prev pointer of the next node does not correctly point to the current node, fix it.
            curNodeNext.getPtr()->prev.store({curNode.getPtr(), curNode.getTag() - 1});
        }

        curNode = {curNodeNext.getPtr(), curNode.getTag() - 1}; // Move to the next node.
    }
}

// Dequeue function to remove elements from the queue
data_type dequeue(queue_t* q) {
    pointer_t head, tail, firstNodePrev;
    node_t* nd_dummy;
    data_type val;

    while (true) {
        head = q->head.load();
        tail = q->tail.load();
        if (firstNodePrev != nullptr) firstNodePrev = head.getPtr()->prev.load();
        if (head != nullptr) val = head.getPtr()->value;

        if (head == q->head.load()) {
            if (val != dummy_val) {
                if (tail != head) {
                    if (firstNodePrev.getTag() != head.getTag()) {
                        fixList(q, tail, head);
                        continue;
                    }
                } else {
                    nd_dummy = new_node();
                    nd_dummy->value = dummy_val;
                    nd_dummy->next.store({tail.getPtr(), tail.getTag() + 1});

                    if (CAS(q->tail, tail, {nd_dummy, tail.getTag() + 1})) {
                        head.getPtr()->prev.store({nd_dummy, tail.getTag()});
                    } else {
                        delete nd_dummy;
                    }
                    continue;
                }
                if (CAS(q->head, head, {firstNodePrev.getPtr(), head.getTag() + 1})) {
                    delete head.getPtr();
                    return val;
                }
            } else {
                if (tail.getPtr() == head.getPtr()) {
                    return -1; // Queue is empty.
                } else {
                    if (firstNodePrev.getTag() != head.getTag()) {
                        fixList(q, tail, head);
                        continue;
                    }
                    CAS(q->head, head, {firstNodePrev.getPtr(), head.getTag() + 1});
                }
            }
        }
    }
    return -1; // Queue is empty.
}

//main for test
int main() {
    // Initialize the queue's head and tail.
    queue_t myQueue;

    // Create multiple threads to simulate concurrent enqueue and dequeue operations.
    std::vector<std::thread> threads;

    // Special threads for creating ABA scenario.
    threads.emplace_back([&myQueue]() {
        // Enqueue a specific value.
        enqueue(&myQueue, 999); 
    });
    threads.emplace_back([&myQueue]() {
        // Dequeue and then re-enqueue the same value.
        dequeue(&myQueue);
        enqueue(&myQueue, 999);
    });

    // Regular enqueue and dequeue operations in other threads.
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&myQueue, i]() {
            enqueue(&myQueue, i);
        });
        threads.emplace_back([&myQueue]() {
            dequeue(&myQueue);
        });
    }

    // Wait for all threads to complete their operations.
    for (auto &th : threads) {
        th.join();
    }


    return 0;
}




// ERROR MESSAGE
// =================================================================
// ==22==ERROR: AddressSanitizer: heap-use-after-free on address 0x603000000010 at pc 0x000000368357 bp 0x7f2f1ef31d90 sp 0x7f2f1ef31d88
// WRITE of size 8 at 0x603000000010 thread T2
//     #1 0x7f2f27ea0792  (/lib/x86_64-linux-gnu/libstdc++.so.6+0xe6792)
//     #2 0x7f2f28030608  (/lib/x86_64-linux-gnu/libpthread.so.0+0x8608)
//     #3 0x7f2f27b61132  (/lib/x86_64-linux-gnu/libc.so.6+0x11f132)
// 0x603000000010 is located 0 bytes inside of 32-byte region [0x603000000010,0x603000000030)
// freed by thread T1 here:
// previously allocated by thread T1 here:
// Thread T2 created by T0 here:
//     #1 0x7f2f27ea0b4b  (/lib/x86_64-linux-gnu/libstdc++.so.6+0xe6b4b)
//     #3 0x7f2f27a66082  (/lib/x86_64-linux-gnu/libc.so.6+0x24082)
// Thread T1 created by T0 here:
//     #1 0x7f2f27ea0b4b  (/lib/x86_64-linux-gnu/libstdc++.so.6+0xe6b4b)
//     #3 0x7f2f27a66082  (/lib/x86_64-linux-gnu/libc.so.6+0x24082)
// Shadow bytes around the buggy address:
//   0x0c067fff7fb0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//   0x0c067fff7fc0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//   0x0c067fff7fd0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//   0x0c067fff7fe0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//   0x0c067fff7ff0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// =>0x0c067fff8000: fa fa[fd]fd fd fd fa fa 00 00 00 fa fa fa fd fd
//   0x0c067fff8010: fd fd fa fa 00 00 00 fa fa fa fa fa fa fa fa fa
//   0x0c067fff8020: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
//   0x0c067fff8030: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
//   0x0c067fff8040: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
//   0x0c067fff8050: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
// Shadow byte legend (one shadow byte represents 8 application bytes):
//   Addressable:           00
//   Partially addressable: 01 02 03 04 05 06 07 
//   Heap left redzone:       fa
//   Freed heap region:       fd
//   Stack left redzone:      f1
//   Stack mid redzone:       f2
//   Stack right redzone:     f3
//   Stack after return:      f5
//   Stack use after scope:   f8
//   Global redzone:          f9
//   Global init order:       f6
//   Poisoned by user:        f7
//   Container overflow:      fc
//   Array cookie:            ac
//   Intra object redzone:    bb
//   ASan internal:           fe
//   Left alloca redzone:     ca
//   Right alloca redzone:    cb
//   Shadow gap:              cc
// ==22==ABORTING