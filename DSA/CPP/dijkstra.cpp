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
#include <utility> // for std::pair
#include <limits> // for std::numeric_limits

using namespace std;

const int INF = numeric_limits<int>::max();


// Initialize:

// For each vertex v in the graph, set distance[v] = infinity and previous[v] = undefined.
// Set distance[source] = 0 because the distance from the source to itself is zero.
// Create a priority queue (min-heap) Q to prioritize vertices with the smallest distance.

// Main Loop:
// While Q is not empty:
// Remove a vertex u from Q with the smallest distance.
// For each neighbor v of u:
// If distance[u] + weight(u, v) < distance[v]:
// Update distance[v] = distance[u] + weight(u, v).
// Update previous[v] = u.
// Update v's position in Q (decrease-key operation).

// Output:
// The distance array holds the shortest distances from the source to all vertices.
// The previous array can be used to reconstruct the shortest paths.

// Data Structures
// Priority Queue: Implemented as a min-heap to efficiently extract the next vertex with the smallest tentative distance. This is crucial for optimizing the algorithm.
// Adjacency List: Represents the graph to efficiently iterate over neighbors of a vertex, particularly useful for sparse graphs.

void dijkstra(const vector<vector<pair<int, int>>>& graph, int source) {
    int n = graph.size();
    vector<int> distance(n, INF), previous(n, -1);
    distance[source] = 0;

    // Min-heap to store (distance, vertex) pairs
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> Q;
    Q.push({0, source});

    while (!Q.empty()) {
        int dist = Q.top().first;
        int u = Q.top().second;
        Q.pop();

        // Skip processing if we've found a better path
        if (dist > distance[u]) continue;

        for (auto& edge : graph[u]) {
            int v = edge.first, weight = edge.second;

            if (distance[u] + weight < distance[v]) {
                distance[v] = distance[u] + weight;
                previous[v] = u;
                Q.push({distance[v], v});
            }
        }
    }

    // Output the distances
    for (int i = 0; i < n; ++i) {
        cout << "Distance from " << source << " to " << i << " is " << distance[i] << endl;
    }
}

int main() {
    // Example graph as an adjacency list: (u, v, weight)
    vector<vector<pair<int, int>>> graph = {
        {{1, 4}, {2, 1}}, // Neighbors of vertex 0
        {{3, 1}},         // Neighbors of vertex 1
        {{1, 2}, {3, 5}}, // Neighbors of vertex 2
        {}                // Neighbors of vertex 3
    };

    int source = 0;
    dijkstra(graph, source);

    return 0;
}






// Time and Space Complexity
// Time Complexity: O((V+E) log V), where V is the number of vertices and E is the number of edges. The log V factor comes from the operations on the priority queue.
// Space Complexity: O(V + E) for storing the graph as an adjacency list, plus O(V) for the distance and previous arrays, resulting in O(V + E) overall.

