#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

// Serial Minimum
int serialMin(vector<int>& arr)
{
    int minVal = arr[0];

    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i] < minVal)
        {
            minVal = arr[i];
        }
    }

    return minVal;
}

// Parallel Minimum
int parallelMin(vector<int>& arr)
{
    int minVal = arr[0];

    #pragma omp parallel for reduction(min:minVal)
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i] < minVal)
        {
            minVal = arr[i];
        }
    }

    return minVal;
}

// Serial Maximum
int serialMax(vector<int>& arr)
{
    int maxVal = arr[0];

    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i] > maxVal)
        {
            maxVal = arr[i];
        }
    }

    return maxVal;
}

// Parallel Maximum
int parallelMax(vector<int>& arr)
{
    int maxVal = arr[0];

    #pragma omp parallel for reduction(max:maxVal)
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i] > maxVal)
        {
            maxVal = arr[i];
        }
    }

    return maxVal;
}

// Serial Sum
long long serialSum(vector<int>& arr)
{
    long long sum = 0;

    for (int i = 0; i < arr.size(); i++)
    {
        sum += arr[i];
    }

    return sum;
}

// Parallel Sum
long long parallelSum(vector<int>& arr)
{
    long long sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < arr.size(); i++)
    {
        sum += arr[i];
    }

    return sum;
}

// Serial Average
double serialAverage(vector<int>& arr)
{
    long long sum = serialSum(arr);

    return (double)sum / arr.size();
}

// Parallel Average
double parallelAverage(vector<int>& arr)
{
    long long sum = parallelSum(arr);

    return (double)sum / arr.size();
}

int main()
{
    int n;

    cout << "Enter number of elements: ";
    cin >> n;

    vector<int> arr(n);

    srand(time(0));

    // Random numbers generate
    for (int i = 0; i < n; i++)
    {
        arr[i] = rand() % 10000;
    }

    // Print first 20 elements only
    cout << "\nFirst 20 Array Elements:\n";

    for (int i = 0; i < min(20, n); i++)
    {
        cout << arr[i] << " ";
    }

    // ---------------- SERIAL ----------------

    double start1 = omp_get_wtime();

    int sMin = serialMin(arr);
    int sMax = serialMax(arr);
    long long sSum = serialSum(arr);
    double sAvg = serialAverage(arr);

    double end1 = omp_get_wtime();

    double sequentialTime = end1 - start1;

    // ---------------- PARALLEL ----------------

    double start2 = omp_get_wtime();

    int pMin = parallelMin(arr);
    int pMax = parallelMax(arr);
    long long pSum = parallelSum(arr);
    double pAvg = parallelAverage(arr);

    double end2 = omp_get_wtime();

    double parallelTime = end2 - start2;

    // ---------------- OUTPUT ----------------

    cout << "\n\n===== SERIAL RESULTS =====";

    cout << "\nMinimum = " << sMin;
    cout << "\nMaximum = " << sMax;
    cout << "\nSum = " << sSum;
    cout << "\nAverage = " << fixed << setprecision(2) << sAvg;

    cout << "\nSequential Time = "
         << sequentialTime * 1000
         << " ms";

    cout << "\n\n===== PARALLEL RESULTS =====";

    cout << "\nMinimum = " << pMin;
    cout << "\nMaximum = " << pMax;
    cout << "\nSum = " << pSum;
    cout << "\nAverage = " << fixed << setprecision(2) << pAvg;

    cout << "\nParallel Time = "
         << parallelTime * 1000
         << " ms";

    // Speedup
    cout << "\n\nSpeedup = ";

    if (parallelTime > 0)
    {
        cout << sequentialTime / parallelTime;
    }
    else
    {
        cout << "Infinity";
    }

    cout << endl;

    return 0;
}

/* Commands-
1st -  g++ Min_Max_Sum_Avg.cpp -fopenmp -o Min_Max_Sum_Avg
2nd - ./Min_Max_Sum_Avg
*/