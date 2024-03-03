#include <mpi.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

// Function to perform counting sort of arr[] according to the digit represented by exp.
void countSort(std::vector<int>& arr, int exp) {
    std::vector<int> output(arr.size());
    int i, count[10] = {0};

    // Store count of occurrences in count[]
    for (i = 0; i < arr.size(); i++)
        count[(arr[i] / exp) % 10]++;

    // Change count[i] so that count[i] now contains actual position of this digit in output[]
    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];

    // Build the output array
    for (i = arr.size() - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }

    // Copy the output array to arr[], so that arr[] now contains sorted numbers according to current digit
    for (i = 0; i < arr.size(); i++)
        arr[i] = output[i];
}


void redistributeElements(std::vector<int>& arr, int exp, int rank, int size) {
    // Calculate the range each process is responsible for based on the current exp
    int digitRange = (10 / size) + (10 % size > 0 ? 1 : 0); // Adjust based on your needs
    
    std::vector<std::vector<int>> toSend(size);
    std::vector<int> receiveCounts(size, 0);

    // Classify elements into buckets for each process
    for (int i = 0; i < arr.size(); ++i) {
        int digit = (arr[i] / exp) % 10;
        int targetProcess = digit / digitRange;
        toSend[targetProcess].push_back(arr[i]);
    }

    // Communicate to each process how many elements they will receive
    for (int i = 0; i < size; ++i) {
        // Prepare counts of elements to send
        int sendCount = toSend[i].size();
        MPI_Allgather(&sendCount, 1, MPI_INT, receiveCounts.data(), 1, MPI_INT, MPI_COMM_WORLD);
    }

    // Prepare for receiving elements
    std::vector<int> recvBuffer;
    for (int count : receiveCounts) {
        recvBuffer.resize(recvBuffer.size() + count);
    }

    // Actually send and receive elements
    int pos = 0;
    for (int i = 0; i < size; ++i) {
        if (i == rank) {
            // Copy local elements directly to the receive buffer
            std::copy(toSend[i].begin(), toSend[i].end(), recvBuffer.begin() + pos);
        } else {
            // Send and receive elements
            MPI_Sendrecv(toSend[i].data(), toSend[i].size(), MPI_INT, i, 0,
                         recvBuffer.data() + pos, receiveCounts[i], MPI_INT, i, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        pos += receiveCounts[i];
    }

    // Update the local array with received elements
    arr = std::move(recvBuffer);
}

void radixSort(std::vector<int>& arr, int rank, int size) {
    // Determine the maximum number to know the number of digits
    int globalMax;
    int localMax = *std::max_element(arr.begin(), arr.end());

    // Get the maximum value among all processes
    MPI_Allreduce(&localMax, &globalMax, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    // Perform counting sort for every digit
    for (int exp = 1; globalMax / exp > 0; exp *= 10) {
        countSort(arr, exp);
        redistributeElements(arr, exp, rank, size);
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Example: Initialize your data depending on MPI rank
    // In a real scenario, data should be distributed among processes

    std::vector<int> data; // Populate this with your data distribution logic

    radixSort(data, rank, size);

    // Optional: Gather sorted data to a single process or distribute the final result as needed

    MPI_Finalize();
    return 0;
}



