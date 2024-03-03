// radixSort
#include <algorithm>
#include <cmath>
#include <vector>

void countingSort(std::vector<int>& arr, int exp) {
    std::vector<int> output(arr.size());
    int count[10] = {0};

    for (int i = 0; i < arr.size(); i++)
        count[(arr[i] / exp) % 10]++;

    for (int i = 1; i < 10; i++)
        count[i] += count[i - 1];

    for (int i = arr.size() - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }

    for (int i = 0; i < arr.size(); i++)
        arr[i] = output[i];
}

void radixSort(std::vector<int>& arr) {
    int maxElement = *std::max_element(arr.begin(), arr.end());

    for (int exp = 1; maxElement / exp > 0; exp *= 10)
        countingSort(arr, exp);
}
