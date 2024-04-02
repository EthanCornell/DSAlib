// Distributed BFS with 2-D Partitioning
// ref :https://en.wikipedia.org/wiki/Parallel_breadth-first_search
#include <algorithm>
#include <iostream>
#include <mpi.h>
#include <numeric>
#include <queue>
#include <vector>

// Global variables for processor grid dimensions
int P, R, C; // Total processors, rows, and columns in processor grid
int rank, dims[2], coord[2], size; // Rank and coordinates (i, j) in grid

// Assuming these global variables are defined and initialized elsewhere
MPI_Comm grid_comm, row_comm, col_comm;
extern std::vector<std::vector<int>>
    local_adj; // The local adjacency matrix part

// Prototype declarations for later use
void setupMPI();
void expandPhase(std::queue<int> &frontier, std::vector<int> &distances);
void foldPhase(std::queue<int> &new_frontier, std::vector<int> &distances);
bool terminationCondition();

void setupMPI() {
  //   MPI_Init(nullptr, nullptr);
  MPI_Comm_size(MPI_COMM_WORLD, &P);
  // Assume R and C are set such that R*C = P
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  R = 1;
  C = P; // P = RC
  std::cout << "R: " << R << ", C: " << C << ", P: " << P << std::endl;

  // Create Cartesian grid
  int dims[2] = {R, C};
  int periods[2] = {0, 0}; // No wrap-around
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &grid_comm);
  if (grid_comm == MPI_COMM_NULL) {
    std::cerr << "Failed to create a valid Cartesian topology. Exiting.\n";
    MPI_Abort(MPI_COMM_WORLD, 1); // Use MPI_Abort to terminate the program
  }
  MPI_Cart_coords(grid_comm, rank, 2, coord);

  // Split the grid communicator into row and column communicators
  MPI_Comm_split(grid_comm, coord[0], coord[1], &row_comm); // Row communicator
  MPI_Comm_split(grid_comm, coord[1], coord[0],
                 &col_comm); // Column communicator
}

// Utility function to calculate submatrix dimensions
void calculateSubmatrixDimensions(int global_rows, int global_cols,
                                  int &local_rows, int &local_cols) {
  local_rows = global_rows / dims[0]; // Assuming evenly divisible
  local_cols = global_cols / dims[1]; // Assuming evenly divisible
}

void distributeGraph(const std::vector<int> &graph, int global_rows,
                     int global_cols) {
  int local_rows, local_cols;
  calculateSubmatrixDimensions(global_rows, global_cols, local_rows,
                               local_cols);

  std::vector<int> local_adj_matrix(local_rows * local_cols);

  if (rank == 0) {
    // Assuming the entire graph is initially stored in a 1D vector in row-major
    // order
    for (int proc = 0; proc < size; ++proc) {
      int proc_coords[2];
      MPI_Cart_coords(grid_comm, proc, 2, proc_coords);

      // Calculate the starting row and column for this processor's submatrix
      int start_row = proc_coords[0] * local_rows;
      int start_col = proc_coords[1] * local_cols;

      // Extract submatrix for this processor
      std::vector<int> submatrix(local_rows * local_cols);
      for (int i = 0; i < local_rows; ++i) {
        for (int j = 0; j < local_cols; ++j) {
          submatrix[i * local_cols + j] =
              graph[(start_row + i) * global_cols + (start_col + j)];
        }
      }

      if (proc == 0) {
        // This is the root processor itself; no need to send
        local_adj_matrix.swap(submatrix);
      } else {
        // Send submatrix to the corresponding processor
        MPI_Send(submatrix.data(), local_rows * local_cols, MPI_INT, proc, 0,
                 MPI_COMM_WORLD);
      }
    }
  } else {
    // All other processors receive their submatrix
    MPI_Recv(local_adj_matrix.data(), local_rows * local_cols, MPI_INT, 0, 0,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  // Now, each processor has its submatrix in `local_adj_matrix`
}

void expandPhase(std::queue<int> &frontier, std::vector<int> &distances) {
  // First, gather the size of the frontier in each processor within the column
  int local_frontier_size = frontier.size();
  std::vector<int> all_frontier_sizes;
  int col_size;
  MPI_Comm_size(col_comm, &col_size);
  all_frontier_sizes.resize(col_size);

  MPI_Allgather(&local_frontier_size, 1, MPI_INT, all_frontier_sizes.data(), 1,
                MPI_INT, col_comm);

  // Calculate the total number of frontier vertices to be exchanged
  int total_frontier_size = 0;
  for (int size : all_frontier_sizes) {
    total_frontier_size += size;
  }

  // Prepare the send buffer with all local frontier vertices
  std::vector<int> send_buffer(local_frontier_size);
  int i = 0;
  while (!frontier.empty()) {
    send_buffer[i++] = frontier.front();
    frontier.pop(); // Pop vertices as we're going to refill the frontier later
  }

  // Calculate displacements for the allgatherv operation
  std::vector<int> displacements(col_size, 0);
  std::partial_sum(all_frontier_sizes.begin(), all_frontier_sizes.end() - 1,
                   displacements.begin() + 1);

  // Allgather the vertices across the column
  std::vector<int> all_frontier_vertices(total_frontier_size);
  MPI_Allgatherv(send_buffer.data(), local_frontier_size, MPI_INT,
                 all_frontier_vertices.data(), all_frontier_sizes.data(),
                 displacements.data(), MPI_INT, col_comm);

  // Refill the frontier with the received vertices (now contains all vertices
  // in the column's frontier)
  for (int vertex : all_frontier_vertices) {
    frontier.push(vertex);
  }

  // Note: At this point, each processor in the column knows about all frontier
  // vertices. We might need to filter these vertices to avoid re-visiting or
  // handle them according to  graph partitioning.
}

// This function checks if all processors have an empty frontier,
// indicating that the BFS traversal is complete.
bool terminationCondition(MPI_Comm comm) {
  int localIsEmpty = 1; // Assuming this processor's frontier is empty
  int globalIsEmpty;

  // Check if the local frontier is empty
  //   localIsEmpty = frontier.empty() ? 1 : 0;

  // Perform an Allreduce operation to check if all frontiers are empty across
  // all processors MPI_LAND is a logical AND operation. If any processor's
  // frontier is not empty, globalIsEmpty will be 0.
  MPI_Allreduce(&localIsEmpty, &globalIsEmpty, 1, MPI_INT, MPI_LAND, comm);

  // If globalIsEmpty is 1, then all processors have empty frontiers, and the
  // BFS is complete
  return globalIsEmpty == 1;
}

void foldPhase(std::queue<int> &new_frontier, std::vector<int> &distances) {
  int row_rank, row_size;
  MPI_Comm_rank(row_comm, &row_rank);
  MPI_Comm_size(row_comm, &row_size);

  // Step 1: Gather the sizes of the new_frontiers from all processors in the
  // row
  int local_new_frontier_size = new_frontier.size();
  std::vector<int> all_new_frontier_sizes(row_size);
  MPI_Allgather(&local_new_frontier_size, 1, MPI_INT,
                all_new_frontier_sizes.data(), 1, MPI_INT, row_comm);

  // Step 2: Prepare the send buffer with all local new frontier vertices
  std::vector<int> send_buffer(local_new_frontier_size);
  int idx = 0;
  while (!new_frontier.empty()) {
    send_buffer[idx++] = new_frontier.front();
    new_frontier.pop();
  }

  // Calculate displacements for the allgatherv operation
  std::vector<int> displacements(row_size, 0);
  std::partial_sum(all_new_frontier_sizes.begin(),
                   all_new_frontier_sizes.end() - 1, displacements.begin() + 1);

  // Step 3: Allgather the new vertices across the row
  int total_new_frontier_size = std::accumulate(
      all_new_frontier_sizes.begin(), all_new_frontier_sizes.end(), 0);
  std::vector<int> all_new_frontier_vertices(total_new_frontier_size);
  MPI_Allgatherv(send_buffer.data(), local_new_frontier_size, MPI_INT,
                 all_new_frontier_vertices.data(),
                 all_new_frontier_sizes.data(), displacements.data(), MPI_INT,
                 row_comm);

  // Step 4: Each processor now has all the new vertices discovered by
  // processors in its row Processors need to filter out vertices that they are
  // responsible for This step depends on graph partitioning logic, for
  // example: for (int vertex : all_new_frontier_vertices) {
  //     if (isResponsibleForVertex(vertex)) { // Implement this function based
  //     on partitioning scheme
  //         new_frontier.push(vertex);
  //     }
  // }
  // Note: The above loop is a placeholder. We'll need to implement logic to
  // determine if a vertex belongs to the processor's sub-graph.
}

void parallelBFS(int source, const std::vector<std::vector<int>> &graph,
                 std::vector<int> &distances) {
  // Initialize distances with -1 to indicate that vertices are unvisited
  distances.assign(graph.size(), -1);

  if (source < 0 || source >= graph.size())
    return; // Check for valid source

  distances[source] = 0; // Distance to the source is 0
  std::queue<int> queue;
  queue.push(source);

  while (!queue.empty()) {
    int current = queue.front();
    queue.pop();

    for (int neighbor : graph[current]) {
      if (distances[neighbor] == -1) { // If unvisited
        distances[neighbor] = distances[current] + 1;
        queue.push(neighbor);
      }
    }
  }

  // Note: This example assumes the entire graph is stored on each processor,
  // which may not align with distributed BFS logic. Adjust as needed for
  // distributed logic.
}

// test cases
std::vector<std::vector<int>> createLinearGraph(int numVertices) {
  std::vector<std::vector<int>> graph(numVertices);
  for (int i = 0; i < numVertices - 1; ++i) {
    graph[i].push_back(i + 1); // Connect current vertex to the next
    graph[i + 1].push_back(i); // Ensure the graph is undirected
  }
  return graph;
}

std::vector<std::vector<int>> createCompleteGraph(int numVertices) {
  std::vector<std::vector<int>> graph(numVertices);
  for (int i = 0; i < numVertices; ++i) {
    for (int j = 0; j < numVertices; ++j) {
      if (i != j) {
        graph[i].push_back(
            j); // Connect every vertex to all others except itself
      }
    }
  }
  return graph;
}

std::vector<std::vector<int>> createSparseGraph(int numVertices,
                                                int branchingFactor) {
  std::vector<std::vector<int>> graph(numVertices);
  int currentVertex = 0;
  for (int i = 1; i < numVertices && currentVertex < numVertices - 1; ++i) {
    for (int j = 0; j < branchingFactor && currentVertex < numVertices - 1;
         ++j) {
      graph[currentVertex].push_back(i);
      graph[i].push_back(currentVertex);
      ++currentVertex;
    }
  }
  return graph;
}

bool verifyBFSResults(const std::vector<int> &distances,
                      const std::vector<int> &expectedDistances) {
  // Assuming distances and expectedDistances are of the same size and contain
  // the distance from the source vertex to every other vertex in the graph.
  // This function checks if the calculated distances match the expected
  // distances.
  if (distances.size() != expectedDistances.size()) {
    std::cerr
        << "Error: Size of distances and expectedDistances does not match.\n";
    return false;
  }

  for (size_t i = 0; i < distances.size(); ++i) {
    if (distances[i] != expectedDistances[i]) {
      std::cerr << "Mismatch found at vertex " << i
                << ": expected distance = " << expectedDistances[i]
                << ", but found distance = " << distances[i] << ".\n";
      return false;
    }
  }

  return true; // All distances match the expected distances
}

void calculateExpectedDistancesSequentially(
    const std::vector<std::vector<int>> &graph, int source,
    std::vector<int> &expectedDistances) {
  std::queue<int> queue;
  queue.push(source);
  expectedDistances.assign(graph.size(), -1); // Initialize distances
  expectedDistances[source] = 0;              // Distance to source is 0

  while (!queue.empty()) {
    int current = queue.front();
    queue.pop();

    for (int neighbor : graph[current]) {
      if (expectedDistances[neighbor] == -1) { // If unvisited
        expectedDistances[neighbor] = expectedDistances[current] + 1;
        queue.push(neighbor);
      }
    }
  }
}

void runBFSTest(const std::vector<std::vector<int>> &graph, int source,
                const std::vector<int> &expectedDistances) {
  std::vector<int> distances(graph.size(), -1); // Initialize distances
  parallelBFS(source, graph, distances);        // Run BFS
  if (verifyBFSResults(distances, expectedDistances)) {
    std::cout << "Test passed\n";
  } else {
    std::cout << "Test failed\n";
  }
}

int main(int argc, char **argv) {
  //   int P, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &P);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Simple setup for demonstration purposes
  //   R = C = static_cast<int>(sqrt(P));
  setupMPI();

  // Example usage of graph creation and testing BFS
  int numVertices = 10; // Adjust based on test scenario
  int branchingFactor = 2;
  if (rank == 0) { // Only the root processor outputs, but all processors should
                   // participate in the test
    std::cout << "Running BFS Test Cases\n";

    // Test 1: Linear Graph
    auto linearGraph = createLinearGraph(numVertices);
    std::vector<int> expectedDistancesForLinear(numVertices);
    for (int i = 0; i < numVertices; ++i)
      expectedDistancesForLinear[i] = i; // Expected distances in a linear graph
    runBFSTest(linearGraph, 0, expectedDistancesForLinear);

    // Test 2: Complete Graph
    auto completeGraph = createCompleteGraph(numVertices);
    std::vector<int> expectedDistancesForComplete(
        numVertices, 1); // Except for the source vertex, every other vertex is
                         // at a distance of 1 in a complete graph

    expectedDistancesForComplete[0] =
        0; // The distance from the source vertex to itself is always 0

    runBFSTest(completeGraph, 0, expectedDistancesForComplete);

    // Test 3: Sparse Graph
    auto sparseGraph = createSparseGraph(numVertices, branchingFactor);
    std::vector<int> expectedDistancesForSparse;
    calculateExpectedDistancesSequentially(
        sparseGraph, 0,
        expectedDistancesForSparse); // Dynamically calculate expected distances

    runBFSTest(sparseGraph, 0, expectedDistancesForSparse);
  }

  MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes reach this point before
                               // finalizing (good practice)

  MPI_Finalize();
  return 0;
}

// compile: mpicxx distributed_bfs_2d.cpp -o distributed_bfs_2d -O3
// execute: mpiexec -n 4 ./distributed_bfs_2d
