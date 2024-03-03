
// oddEvenTranspositionSort
#include <iostream>
#include <vector>
#include <omp.h>

void oddEvenTranspositionSort(std::vector<int>& arr) {
    int n = arr.size();
    bool isSorted = false;

    while (!isSorted) {
        isSorted = true;

        // Odd phase
        #pragma omp parallel for
        for (int i = 1; i <= n - 2; i += 2) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                isSorted = false;
            }
        }

        // Even phase
        #pragma omp parallel for
        for (int i = 0; i <= n - 2; i += 2) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                isSorted = false;
            }
        }
    }
}

int main() {
    std::vector<int> arr = {34, 8, 50, 12, 5, 40, 15, 1};

    oddEvenTranspositionSort(arr);

    for (int i = 0; i < arr.size(); i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
