
// Iterative Bitonic Recursive (IBR) Bitonic Sort 
#include <iostream>
#include <vector>
#include <omp.h>

void compAndSwap(std::vector<int>& arr, int i, int j, int dir) {
    if (dir == (arr[i] > arr[j])) {
        std::swap(arr[i], arr[j]);
    }
}

void bitonicMerge(std::vector<int>& arr, int low, int cnt, int dir) {
    if (cnt > 1) {
        int k = cnt / 2;
        for (int i = low; i < low + k; i++) {
            compAndSwap(arr, i, i + k, dir);
        }
        bitonicMerge(arr, low, k, dir);
        bitonicMerge(arr, low + k, k, dir);
    }
}

// Iterative function to sort a bitonic sequence
void bitonicSortIterative(std::vector<int>& arr, int n, int up) {
    // Iterate over the sequence lengths
    for (int size = 2; size <= n; size = 2 * size) {
        // Launch parallel sections for each half of the array
        #pragma omp parallel for
        for (int i = 0; i < n; i += size) {
            int dir = ((i / size) % 2) == 0 ? up : !up;
            bitonicMerge(arr, i, size, dir);
        }
    }
}

int main() {
    std::vector<int> arr = {3, 7, 4, 8, 6, 2, 1, 5};
    int n = arr.size();
    int up = 1; // Indicates sorting in ascending order

    bitonicSortIterative(arr, n, up);

    for (int i = 0; i < n; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
