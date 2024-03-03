
// Multistep Bitonic Sort 
#include <iostream>
#include <vector>
#include <omp.h>

// Function to compare and swap elements
void compAndSwap(std::vector<int>& arr, int i, int j, int dir) {
    if ((dir == 1 && arr[i] > arr[j]) || (dir == 0 && arr[i] < arr[j])) {
        std::swap(arr[i], arr[j]);
    }
}

// Recursive function to generate a bitonic sequence
void bitonicMerge(std::vector<int>& arr, int low, int cnt, int dir) {
    if (cnt > 1) {
        int k = cnt / 2;
        #pragma omp parallel for
        for (int i = low; i < low + k; i++) {
            compAndSwap(arr, i, i + k, dir);
        }
        bitonicMerge(arr, low, k, dir);
        bitonicMerge(arr, low + k, k, dir);
    }
}

// Function to sort a bitonic sequence in ascending order if dir is 1,
// otherwise in descending order
void bitonicSort(std::vector<int>& arr, int low, int cnt, int dir) {
    if (cnt > 1) {
        int k = cnt / 2;

        // Sort in ascending order since dir here is 1
        bitonicSort(arr, low, k, 1);

        // Sort in descending order since dir here is 0
        bitonicSort(arr, low + k, k, 0);

        // Merge the two sequences into a single sequence
        bitonicMerge(arr, low, cnt, dir);
    }
}

// Caller function for bitonic sort
void sort(std::vector<int>& arr, int n, int up) {
    bitonicSort(arr, 0, n, up);
}

int main() {
    std::vector<int> arr = {3, 7, 4, 8, 6, 2, 1, 5};
    int n = arr.size();
    int up = 1; // Indicates sorting in ascending order

    sort(arr, n, up);

    for (int i = 0; i < n; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
