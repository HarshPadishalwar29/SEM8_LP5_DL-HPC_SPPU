#include <iostream>
#include <vector>
#include <algorithm>
#include <mpi.h>

using namespace std;

// Sequential Quick Sort
void quickSort(vector<int>& arr, int low, int high) {

    if (low < high) {

        int pivot = arr[high];
        int i = low - 1;

        for (int j = low; j < high; j++) {

            if (arr[j] < pivot) {

                i++;
                swap(arr[i], arr[j]);
            }
        }

        swap(arr[i + 1], arr[high]);

        int pi = i + 1;

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main(int argc, char* argv[]) {

    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 16;

    vector<int> arr;

    // Root process initializes array
    if (rank == 0) {

        arr = {45, 12, 78, 34,
               23, 89, 11, 67,
               90, 54, 21, 43,
               76, 32, 65, 10};

        cout << "Original Array:\n";

        for (int x : arr) {
            cout << x << " ";
        }

        cout << endl;
    }

    // Divide data among processes
    int local_n = n / size;

    vector<int> local_arr(local_n);

    MPI_Scatter(arr.data(),
                local_n,
                MPI_INT,
                local_arr.data(),
                local_n,
                MPI_INT,
                0,
                MPI_COMM_WORLD);

    // Measure parallel quicksort time
    double start = MPI_Wtime();

    quickSort(local_arr, 0, local_n - 1);

    double end = MPI_Wtime();

    double local_time = end - start;
    double max_time;

    // Get maximum execution time
    MPI_Reduce(&local_time,
               &max_time,
               1,
               MPI_DOUBLE,
               MPI_MAX,
               0,
               MPI_COMM_WORLD);

    // Gather sorted subarrays
    vector<int> gathered;

    if (rank == 0) {
        gathered.resize(n);
    }

    MPI_Gather(local_arr.data(),
               local_n,
               MPI_INT,
               gathered.data(),
               local_n,
               MPI_INT,
               0,
               MPI_COMM_WORLD);

    // Final merge at root
    if (rank == 0) {

        sort(gathered.begin(), gathered.end());

        cout << "\nSorted Array:\n";

        for (int x : gathered) {
            cout << x << " ";
        }

        cout << "\n\nParallel Quicksort Time: "
             << max_time
             << " seconds\n";
    }

    MPI_Finalize();

    return 0;
}

/* Commands-
1st -  g++ QuickSort.cpp -fopenmp -o QuickSort
2nd - ./QuickSort
*/