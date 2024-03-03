// cycleSort
#include <vector>

void cycleSort(std::vector<int>& arr) {
    int writes = 0;
    for (int cycle_start = 0; cycle_start <= arr.size() - 2; cycle_start++) {
        int item = arr[cycle_start];
        int pos = cycle_start;
        for (int i = cycle_start + 1; i < arr.size(); i++)
            if (arr[i] < item)
                pos++;

        if (pos == cycle_start)
            continue;

        while (item == arr[pos])
            pos += 1;

        if (pos != cycle_start) {
            std::swap(item, arr[pos]);
            writes++;
        }

        while (pos != cycle_start) {
            pos = cycle_start;
            for (int i = cycle_start + 1; i < arr.size(); i++)
                if (arr[i] < item)
                    pos += 1;

            while (item == arr[pos])
                pos += 1;

            if (item != arr[pos]) {
              std::swap(item, arr[pos]);
              writes++;
            }
        }
    }
}
