// Distributed BFS with 1-D Partitioning
#include <algorithm>
#include <iostream>
#include <limits>
#include <mpi.h>
#include <vector>

std::vector<int> graph[] = {
    {1, 2},    // Neighbors of vertex 0
    {0, 3, 4}, // Neighbors of vertex 1
    {0, 5},    // Neighbors of vertex 2
    {1, 6},    // Neighbors of vertex 3
    {1, 6, 7}, // Neighbors of vertex 4
    {2, 7},    // Neighbors of vertex 5
    {3, 4},    // Neighbors of vertex 6
    {4, 5}     // Neighbors of vertex 7
};

void distributed_bfs(int start_vertex, std::vector<int> &levels, int rank,
                     int world_size) {
  int num_vertices = sizeof(graph) / sizeof(graph[0]);
  std::vector<int> local_levels(num_vertices, std::numeric_limits<int>::max());
  // Initialize local BFS levels
  local_levels[start_vertex] = 0;
  std::vector<int> level_changes(num_vertices, 0);

  // Main BFS loop
  bool changed;
  do {
    changed = false;
    fill(level_changes.begin(), level_changes.end(), 0);
    // Check each vertex to see if it is at the current level.
    for (int vertex = 0; vertex < num_vertices; ++vertex) {
      if (local_levels[vertex] == std::numeric_limits<int>::max()) {
        continue; // Skip vertices that are not yet reached.
      }
      for (int neighbor : graph[vertex]) {
        if (local_levels[neighbor] == std::numeric_limits<int>::max()) {
          level_changes[neighbor] = local_levels[vertex] + 1;
          changed = true;
        }
      }
    }
    // Apply changes if there are any
    for (int i = 0; i < num_vertices; ++i) {
      if (level_changes[i] != 0) {
        local_levels[i] = level_changes[i];
      }
    }
    // Communicate the changes to all processors
    MPI_Allreduce(MPI_IN_PLACE, local_levels.data(), num_vertices, MPI_INT,
                  MPI_MIN, MPI_COMM_WORLD);
    MPI_Allreduce(MPI_IN_PLACE, &changed, 1, MPI_C_BOOL, MPI_LOR,
                  MPI_COMM_WORLD);
  } while (changed);
  // Update the global levels vector with the local levels
  levels = local_levels;
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int world_size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int num_vertices = sizeof(graph) / sizeof(graph[0]);
  std::vector<int> levels(num_vertices, std::numeric_limits<int>::max());

  // Assuming vertex 0 is the source vertex for BFS
  distributed_bfs(0, levels, rank, world_size);

  // Print the BFS levels
  if (rank == 0) {
    for (int i = 0; i < num_vertices; ++i) {
      std::cout << "Vertex " << i << " has level " << levels[i] << std::endl;
    }
  }

  MPI_Finalize();
  return 0;
}

// compile: mpicxx distributed_bfs.cpp -o distributed_bfs -O3
// execute: mpiexec -n 4 ./distributed_bfs
