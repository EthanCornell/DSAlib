#include <mpi.h>
#include <iostream>
#include <vector>
#include <queue>

// Distributed BFS with 1-D Partitioning
std::vector<int> graph[] = {
    {1, 2},        // Neighbors of vertex 0
    {0, 3, 4},     // Neighbors of vertex 1
    {0, 5},        // Neighbors of vertex 2
    {1, 6},        // Neighbors of vertex 3
    {1, 6, 7},     // Neighbors of vertex 4
    {2, 7},        // Neighbors of vertex 5
    {3, 4},        // Neighbors of vertex 6
    {4, 5}         // Neighbors of vertex 7
};

// Function to perform distributed BFS
void distributed_bfs(int world_rank, int world_size) {
    int total_vertices = sizeof(graph) / sizeof(graph[0]);
    int vertices_per_process = total_vertices / world_size; // Assuming a total of 4 vertices for simplicity

    std::queue<int> q;
    bool visited[total_vertices] = {false}; // Tracking visited vertices, assuming 4 vertices for simplicity
    
    // Start BFS from vertex 0 in process 0
    if (world_rank == 0) {
        q.push(0); // Assuming BFS starts from vertex 0
    }

    while (!q.empty()) {
        int current_vertex = q.front();
        q.pop();

        // Check if current_vertex is part of the current process's responsibility
        if (current_vertex / vertices_per_process == world_rank) {
            if (!visited[current_vertex]) {
                visited[current_vertex] = true;
                std::cout << "Process " << world_rank << " visiting vertex " << current_vertex << std::endl;

                // Add unvisited neighbors to the queue
                for (int neighbor : graph[current_vertex]) {
                    if (!visited[neighbor]) {
                        q.push(neighbor);
                    }
                }
            }
        } else {
            // If the vertex is not part of this process, send it to the responsible process
            MPI_Send(&current_vertex, 1, MPI_INT, current_vertex / vertices_per_process, 0, MPI_COMM_WORLD);
        }

        // Check for incoming vertices from other processes
        MPI_Status status;
        int incoming_vertex;
        if (MPI_Recv(&incoming_vertex, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status) == MPI_SUCCESS) {
            q.push(incoming_vertex);
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Total number of processes

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // The current process ID

    // Call the distributed BFS function
    distributed_bfs(world_rank, world_size);

    MPI_Finalize();
    return 0;
}


// compile: mpicxx distributed_bfs.cpp -o distributed_bfs -O3
// execute: mpiexec -n 4 ./distributed_bfs


