#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>

void heapify(std::vector<int>& arr, int n, int i) {
    int largest = i; // Initialize largest as root
    int l = 2 * i + 1; // left = 2*i + 1
    int r = 2 * i + 2; // right = 2*i + 2

    // If left child is larger than root
    if (l < n && arr[l] > arr[largest])
        largest = l;

    // If right child is larger than largest so far
    if (r < n && arr[r] > arr[largest])
        largest = r;

    // If largest is not root
    if (largest != i) {
        std::swap(arr[i], arr[largest]);

        // Recursively heapify the affected sub-tree
        heapify(arr, n, largest);
    }
}

void buildHeapParallel(std::vector<int>& arr, int n) {
    // Build heap (rearrange array)
    #pragma omp parallel for
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);
}

void heapsort(std::vector<int>& arr) {
    int n = arr.size();

    buildHeapParallel(arr, n);

    // One by one extract an element from heap
    for (int i = n - 1; i > 0; i--) {
        // Move current root to end
        std::swap(arr[0], arr[i]);

        // call max heapify on the reduced heap
        heapify(arr, i, 0);
    }
}

int main() {
    std::vector<int> arr = {12, 11, 13, 5, 6, 7};

    heapsort(arr);

    std::cout << "Sorted array is \n";
    for(int i : arr)
        std::cout << i << " ";
    std::cout << std::endl;

    return 0;
}


// g++ -pg -fopenmp ParallelHeapsort.cpp -o ParallelHeapsort -O3 && ./ParallelHeapsort
