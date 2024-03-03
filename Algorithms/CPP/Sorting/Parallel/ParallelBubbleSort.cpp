
// Parallel Bubble Sort using OpenMP
#include <omp.h>
#include <vector>
#include <iostream>

void parallelBubbleSort(std::vector<int>& arr) {
    int n = arr.size();
    bool swapped;
    do {
        swapped = false;
        #pragma omp parallel for
        for (int i = 1; i < n; i += 2) { // Handle odd-even pairs
            if (arr[i-1] > arr[i]) {
                std::swap(arr[i-1], arr[i]);
                swapped = true;
            }
        }

        #pragma omp parallel for
        for (int i = 2; i < n; i += 2) { // Handle even-odd pairs
            if (arr[i-1] > arr[i]) {
                std::swap(arr[i-1], arr[i]);
                swapped = true;
            }
        }
    } while (swapped);
}

int main() {
    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90};

    parallelBubbleSort(arr);

    for (int i = 0; i < arr.size(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    return 0;
}
