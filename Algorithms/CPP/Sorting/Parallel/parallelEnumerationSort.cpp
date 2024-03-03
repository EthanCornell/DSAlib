
// parallelEnumerationSort
#include <iostream>
#include <vector>
#include <omp.h>

void parallelEnumerationSort(std::vector<int>& arr) {
    int n = arr.size();
    std::vector<int> sortedArr(n);

    // Parallelize this outer loop
    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        int rank = 0;
        for (int j = 0; j < n; ++j) {
            if (arr[j] < arr[i] || (arr[j] == arr[i] && j < i)) {
                rank++;
            }
        }
        sortedArr[rank] = arr[i];
    }

    // Copy the sorted array back to the original array
    for (int i = 0; i < n; ++i) {
        arr[i] = sortedArr[i];
    }
}

int main() {
    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90};

    parallelEnumerationSort(arr);

    for (int i = 0; i < arr.size(); i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
