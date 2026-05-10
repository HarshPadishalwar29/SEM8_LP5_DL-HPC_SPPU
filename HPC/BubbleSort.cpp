#include <iostream>
#include <chrono>
#include <cstdlib>
#include <omp.h>
#include <iomanip>
#include <algorithm>
#include <vector>

using namespace std;
using namespace chrono;

void print_array(vector<int>& arr)
{
    cout << "Array: [ ";
    for (int x : arr)
    {
        cout << x << " ";
    }
    cout << "]\n";
}

void sequential_bubble_sort(vector<int>& arr, double &seq_time)
{
    vector<int> temp = arr;

    auto start = high_resolution_clock::now();

    for (int i = 0; i < temp.size() - 1; i++)
    {
        for (int j = 0; j < temp.size() - i - 1; j++)
        {
            if (temp[j] > temp[j + 1])
            {
                swap(temp[j], temp[j + 1]);
            }
        }
    }

    auto end = high_resolution_clock::now();
    seq_time = duration<double>(end - start).count();

    cout << "\nSequential Bubble Sort Time: "
         << fixed << setprecision(10) << seq_time << " s\n";

    print_array(temp);
}

void parallel_bubble_sort(vector<int>& arr, double seq_time)
{
    vector<int> temp = arr;

    auto start = high_resolution_clock::now();

    bool sorted = false;

    while (!sorted)
    {
        sorted = true;

        #pragma omp parallel for shared(temp, sorted)
        for (int i = 1; i < temp.size() - 1; i += 2)
        {
            if (temp[i] > temp[i + 1])
            {
                swap(temp[i], temp[i + 1]);
                sorted = false;
            }
        }

        #pragma omp parallel for shared(temp, sorted)
        for (int i = 0; i < temp.size() - 1; i += 2)
        {
            if (temp[i] > temp[i + 1])
            {
                swap(temp[i], temp[i + 1]);
                sorted = false;
            }
        }
    }

    auto end = high_resolution_clock::now();
    double par_time = duration<double>(end - start).count();

    cout << "\nParallel Bubble Sort Time: "
         << fixed << setprecision(10) << par_time << " s\n";

    print_array(temp);

    cout << "\nSpeedup: "
         << fixed << setprecision(10) << seq_time / par_time << "\n";
}

int main()
{
    int size;

    cout << "Enter size of array: ";
    cin >> size;

    vector<int> arr(size);

    for (int i = 0; i < size; i++)
    {
        arr[i] = rand() % size;
    }

    cout << "\nInput Array:\n";
    print_array(arr);

    double seq_time;

    sequential_bubble_sort(arr, seq_time);
    parallel_bubble_sort(arr, seq_time);

    return 0;
}

/* Commands-
1st -  g++ BubbleSort.cpp -fopenmp -o BubbleSort
2nd - ./BubbleSort
*/