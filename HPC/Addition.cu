#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cuda_runtime.h>

#define N 1000000

int A[N], B[N], C[N];

// CUDA Kernel
__global__ void vectorAddition(int *A, int *B, int *C)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    if (i < N)
    {
        C[i] = A[i] + B[i];
    }
}

// Initialize vectors
void initialize()
{
    for (int i = 0; i < N; i++)
    {
        A[i] = rand() % 100;
        B[i] = rand() % 100;
    }
}

// Sequential Addition
void sequentialAddition()
{
    for (int i = 0; i < N; i++)
    {
        C[i] = A[i] + B[i];
    }
}

// Parallel Addition using CUDA
void parallelAddition()
{
    int *d_A, *d_B, *d_C;

    int size = N * sizeof(int);

    // Allocate GPU memory
    cudaMalloc((void**)&d_A, size);
    cudaMalloc((void**)&d_B, size);
    cudaMalloc((void**)&d_C, size);

    // Copy CPU data to GPU
    cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B, size, cudaMemcpyHostToDevice);

    // Kernel launch
    int threadsPerBlock = 256;

    int blocksPerGrid =
        (N + threadsPerBlock - 1)
        / threadsPerBlock;

    vectorAddition<<<blocksPerGrid,
                     threadsPerBlock>>>
                     (d_A, d_B, d_C);

    cudaDeviceSynchronize();

    // Copy result back to CPU
    cudaMemcpy(C,
               d_C,
               size,
               cudaMemcpyDeviceToHost);

    // Free GPU memory
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
}

int main()
{
    srand(time(0));

    initialize();

    // ---------------- Sequential Timing ----------------

    clock_t start1 = clock();

    sequentialAddition();

    clock_t end1 = clock();

    double time_seq =
        (double)(end1 - start1)
        / CLOCKS_PER_SEC;

    // ---------------- Parallel Timing ----------------

    cudaEvent_t start, stop;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);

    parallelAddition();

    cudaEventRecord(stop);

    cudaEventSynchronize(stop);

    float time_par_ms;

    cudaEventElapsedTime(&time_par_ms,
                         start,
                         stop);

    // Convert ms -> seconds
    double time_par =
        time_par_ms / 1000.0;

    // ---------------- Output ----------------

    printf("\nVector Addition Result:\n\n");

    for (int i = 0; i < 10; i++)
    {
        printf("%d + %d = %d\n",
               A[i],
               B[i],
               C[i]);
    }

    // ---------------- Performance ----------------

    printf("\n----- PERFORMANCE -----\n");

    printf("Sequential Time = %f seconds\n",
           time_seq);

    printf("Parallel CUDA Time = %f seconds\n",
           time_par);

    // Speedup
    if (time_par > 0)
    {
        printf("Speedup = %f\n",
               time_seq / time_par);
    }
    else
    {
        printf("Speedup = Infinity\n");
    }

    return 0;
}

/*Commands- 
1st - nvcc Addition.cu -o Addition
2nd - ./Addition
*/