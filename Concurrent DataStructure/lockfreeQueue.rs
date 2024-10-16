/*
BSD 3-Clause License

Copyright (c) 2024, I-Hsuan Huang
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of I-Hsuan Huang nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

use std::sync::atomic::{AtomicPtr, Ordering};
use std::ptr;
use std::sync::Arc;
use std::thread;

struct Node<T> {
    data: Option<T>,
    next: AtomicPtr<Node<T>>,
}

pub struct LockFreeQueue<T> {
    head: AtomicPtr<Node<T>>,
    tail: AtomicPtr<Node<T>>,
}

impl<T> LockFreeQueue<T> {
    pub fn new() -> Self {
        // Create a dummy node to initialize the queue
        let dummy = Box::into_raw(Box::new(Node {
            data: None,
            next: AtomicPtr::new(ptr::null_mut()),
        }));

        LockFreeQueue {
            head: AtomicPtr::new(dummy),
            tail: AtomicPtr::new(dummy),
        }
    }

    pub fn enqueue(&self, value: T) {
        // Create a new node to hold the value
        let new_node = Box::into_raw(Box::new(Node {
            data: Some(value),
            next: AtomicPtr::new(ptr::null_mut()),
        }));

        loop {
            let tail = self.tail.load(Ordering::Acquire);
            let next = unsafe { (*tail).next.load(Ordering::Acquire) };

            if next.is_null() {
                if unsafe { (*tail).next.compare_exchange(ptr::null_mut(), new_node, Ordering::Release, Ordering::Relaxed) }.is_ok() {
                    self.tail.compare_exchange(tail, new_node, Ordering::Release, Ordering::Relaxed).unwrap();
                    break;
                }
            } else {
                // Move the tail pointer forward
                self.tail.compare_exchange(tail, next, Ordering::Release, Ordering::Relaxed).unwrap();
            }
        }
    }

    pub fn dequeue(&self) -> Option<T> {
        loop {
            let head = self.head.load(Ordering::Acquire);
            let tail = self.tail.load(Ordering::Acquire);
            let next = unsafe { (*head).next.load(Ordering::Acquire) };

            if head == tail {
                if next.is_null() {
                    return None; // Queue is empty
                }
                // Move the tail pointer forward
                self.tail.compare_exchange(tail, next, Ordering::Release, Ordering::Relaxed).unwrap();
            } else if let Some(data) = unsafe { (*next).data.take() } {
                self.head.compare_exchange(head, next, Ordering::Release, Ordering::Relaxed).unwrap();
                // Free the old head node
                unsafe { Box::from_raw(head) };
                return Some(data);
            }
        }
    }
}

impl<T> Drop for LockFreeQueue<T> {
    fn drop(&mut self) {
        while self.dequeue().is_some() {} // Empty the queue
        let head = self.head.load(Ordering::Relaxed);
        unsafe { Box::from_raw(head) }; // Free the final dummy node
    }
}


fn main() {
    let queue = Arc::new(LockFreeQueue::new());

    // Basic enqueue and dequeue operations
    println!("Test Case 1: Basic Operations");
    queue.enqueue(1);
    queue.enqueue(2);
    assert_eq!(queue.dequeue(), Some(1));
    assert_eq!(queue.dequeue(), Some(2));
    assert_eq!(queue.dequeue(), None);
    println!("Basic Operations Passed\n");

    // Handling multiple dequeue on an empty queue
    println!("Test Case 2: Dequeue from Empty Queue");
    assert_eq!(queue.dequeue(), None);
    println!("Dequeue from Empty Queue Passed\n");

    // Concurrency Test: Multiple producers and consumers
    println!("Test Case 3: Concurrency Test with Multiple Threads");
    let queue_clone1 = Arc::clone(&queue);
    let producer = thread::spawn(move || {
        for i in 0..10 {
            queue_clone1.enqueue(i);
            println!("Produced: {}", i);
        }
    });

    let queue_clone2 = Arc::clone(&queue);
    let consumer = thread::spawn(move || {
        for _ in 0..10 {
            if let Some(value) = queue_clone2.dequeue() {
                println!("Consumed: {}", value);
            }
        }
    });

    producer.join().unwrap();
    consumer.join().unwrap();
    assert_eq!(queue.dequeue(), None); // Queue should be empty
    println!("Concurrency Test Passed\n");

    // Mixed enqueue and dequeue operations
    println!("Test Case 4: Mixed Operations");
    queue.enqueue(100);
    queue.enqueue(200);
    assert_eq!(queue.dequeue(), Some(100));
    queue.enqueue(300);
    assert_eq!(queue.dequeue(), Some(200));
    assert_eq!(queue.dequeue(), Some(300));
    assert_eq!(queue.dequeue(), None);
    println!("Mixed Operations Passed\n");

    // Stress Test: Many enqueue and dequeue operations
    println!("Test Case 5: Stress Test with Large Number of Operations");
    let queue_clone3 = Arc::clone(&queue);
    let stress_producer = thread::spawn(move || {
        for i in 0..1000 {
            queue_clone3.enqueue(i);
        }
    });

    let queue_clone4 = Arc::clone(&queue);
    let stress_consumer = thread::spawn(move || {
        for _ in 0..1000 {
            queue_clone4.dequeue();
        }
    });

    stress_producer.join().unwrap();
    stress_consumer.join().unwrap();
    assert_eq!(queue.dequeue(), None); // Queue should be empty
    println!("Stress Test Passed\n");

    println!("All Test Cases Passed Successfully!");
}
