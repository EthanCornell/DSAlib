// Parallel Bitonic Sort using OpenMP
#include <omp.h>
#include <algorithm>
#include <vector>
#include <iostream>

void bitonic_merge(std::vector<int>& arr, int low, int cnt, bool dir) {
    if (cnt > 1) {
        int k = cnt / 2;
        for (int i = low; i < low + k; i++) {
            if (dir == (arr[i] > arr[i + k])) {
                std::swap(arr[i], arr[i + k]);
            }
        }
        bitonic_merge(arr, low, k, dir);
        bitonic_merge(arr, low + k, k, dir);
    }
}

void bitonic_sort_rec(std::vector<int>& arr, int low, int cnt, bool dir) {
    if (cnt > 1) {
        int k = cnt / 2;

        #pragma omp parallel sections
        {
            #pragma omp section
            bitonic_sort_rec(arr, low, k, true);

            #pragma omp section
            bitonic_sort_rec(arr, low + k, k, false);
        }

        bitonic_merge(arr, low, cnt, dir);
    }
}

void bitonic_sort(std::vector<int>& arr) {
    int n = arr.size();
    int up = 1; // true: sort in ascending order

    #pragma omp parallel
    {
        #pragma omp single
        bitonic_sort_rec(arr, 0, n, up);
    }
}

int main() {
    std::vector<int> arr = {10, 7, 8, 9, 1, 5, 2, 3};
    bitonic_sort(arr);

    for(int i : arr) {
        std::cout << i << " ";
    }

    return 0;
}
