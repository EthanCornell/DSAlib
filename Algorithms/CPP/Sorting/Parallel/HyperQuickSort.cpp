#include <omp.h>
#include <vector>
#include <iostream>
#include <algorithm>

int partition(std::vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return (i + 1);
}

void quickSortParallel(std::vector<int>& arr, int low, int high, int depth = 0) {
    if (low < high) {
        int pi = partition(arr, low, high);

        // Parallelize the two halves of the array if we're not too deep in the recursion
        if (depth < 4) { // Limit the depth of parallelism
            #pragma omp parallel sections
            {
                #pragma omp section
                { quickSortParallel(arr, low, pi - 1, depth + 1); }

                #pragma omp section
                { quickSortParallel(arr, pi + 1, high, depth + 1); }
            }
        } else { // Sequential execution for deeper levels
            quickSortParallel(arr, low, pi - 1, depth + 1);
            quickSortParallel(arr, pi + 1, high, depth + 1);
        }
    }
}

int main() {
    std::vector<int> arr = {10, 7, 8, 9, 1, 5, 3, 6, 4, 2};
    int n = arr.size();

    #pragma omp parallel
    {
        #pragma omp single nowait
        { quickSortParallel(arr, 0, n - 1); }
    }

    for (int i = 0; i < n; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
