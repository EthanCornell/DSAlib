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

#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <functional>

class ConcurrentBFS {
private:
    // Graph representation and visited flag for each node.
    std::vector<std::vector<int>> graph;
    std::vector<std::atomic<bool>> visited;
    
    // Queue for BFS and mutex for synchronization.
    std::queue<int> queue;
    std::mutex queue_mutex;
    
    // Condition variable to manage worker thread synchronization.
    std::condition_variable cv;
    
    // Atomic variables to manage work status and termination condition.
    std::atomic<int> working;
    std::atomic<bool> done;

    // Function to process a node and add its neighbors to the queue.
    void processNode(int node) {
        for (int neighbor : graph[node]) {
            // If neighbor not visited, mark as visited and add to queue.
            if (!visited[neighbor].exchange(true)) {
                std::lock_guard<std::mutex> guard(queue_mutex);
                queue.push(neighbor);
                cv.notify_one();
            }
        }
    }

    // Worker function run by each thread.
    void worker() {
        while (true) {
            std::unique_lock<std::mutex> lock(queue_mutex);
            // Wait until work is available or done is signaled.
            cv.wait(lock, [this] { return !queue.empty() || done; });

            if (queue.empty()) {
                if (done) break; // Exit loop if no work and done is true.
                continue;
            }

            int node = queue.front();
            queue.pop();
            working++; // Increment active worker count.
            lock.unlock();

            processNode(node); // Process the current node.

            lock.lock();
            working--; // Decrement active worker count.
            // Signal done if no workers active and no nodes left to process.
            if (working == 0 && queue.empty()) {
                done = true;
                cv.notify_all();
            }
        }
    }

public:
    // Constructor initializing the graph and atomic variables.
    ConcurrentBFS(const std::vector<std::vector<int>>& graph) 
        : graph(graph), visited(graph.size()), working(0), done(false) {
    }

    // Function to run the BFS starting from a given node.
    void run(int start) {
        std::fill(visited.begin(), visited.end(), false);
        visited[start] = true;
        queue.push(start);

        // Create and start worker threads based on hardware concurrency.
        std::vector<std::thread> workers;
        int numThreads = std::thread::hardware_concurrency();
        for (int i = 0; i < numThreads; ++i) {
            workers.emplace_back(&ConcurrentBFS::worker, this);
        }

        // Wait for all worker threads to complete.
        for (auto& worker : workers) {
            worker.join();
        }
    }
};


// Time Complexity
// O(V + E): Where V is the number of vertices and E is the number of edges in the graph.
// The concurrent nature of the algorithm aims to speed up the processing by utilizing multiple cores. However, the worst-case time complexity remains O(V + E) because every vertex and edge is processed once.
// Space Complexity
// O(V + E): For storing the graph.
// O(V): For the visited array.
// O(V): Maximum size of the queue.
// Total: O(V + E) + O(V) + O(V) ≈ O(V + E), assuming the graph storage dominates.
// Optimizations and Efficiency
// Concurrency: Utilizes multiple threads for parallel processing.
// Dynamic Thread Allocation: Adapts the number of threads to the system's capabilities.
// Atomic Operations: Reduces the overhead of locking for visited checks.
// Condition Variables: Efficient thread synchronization, reducing idle time.
// Reduced Lock Contention: Minimizes the duration for which locks are held.

// Example usage
int main() {
    std::vector<std::vector<int>> graph = {
        {1, 2},
        {0, 3},
        {0, 3},
        {1, 2}
    };

    ConcurrentBFS bfs(graph);
    bfs.run(0); // Start BFS from node 0

    return 0;
}
