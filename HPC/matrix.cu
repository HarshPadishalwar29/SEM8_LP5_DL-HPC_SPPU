#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cuda_runtime.h>

#define N 1000

int A[N][N], B[N][N];
int C_seq[N][N], C_par[N][N];

// CUDA Kernel
__global__ void matrixMultiply(int *A,
                               int *B,
                               int *C,
                               int n)
{
    int row =
        blockIdx.y * blockDim.y
        + threadIdx.y;

    int col =
        blockIdx.x * blockDim.x
        + threadIdx.x;

    if (row < n && col < n)
    {
        int sum = 0;

        for (int k = 0; k < n; k++)
        {
            sum += A[row * n + k] *
                   B[k * n + col];
        }

        C[row * n + col] = sum;
    }
}

// Initialize matrices
void initialize()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }
    }
}

// Sequential Matrix Multiplication
void sequentialMatrixMultiply()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int sum = 0;

            for (int k = 0; k < N; k++)
            {
                sum += A[i][k] * B[k][j];
            }

            C_seq[i][j] = sum;
        }
    }
}

int main()
{
    srand(time(0));

    initialize();

    // ---------------- Sequential Timing ----------------

    clock_t start_seq = clock();

    sequentialMatrixMultiply();

    clock_t end_seq = clock();

    double sequentialTime =
        (double)(end_seq - start_seq)
        / CLOCKS_PER_SEC;

    // ---------------- GPU Setup ----------------

    int *d_A, *d_B, *d_C;

    int size = N * N * sizeof(int);

    // Allocate GPU memory
    cudaMalloc((void**)&d_A, size);
    cudaMalloc((void**)&d_B, size);
    cudaMalloc((void**)&d_C, size);

    // Copy matrices to GPU
    cudaMemcpy(d_A,
               A,
               size,
               cudaMemcpyHostToDevice);

    cudaMemcpy(d_B,
               B,
               size,
               cudaMemcpyHostToDevice);

    // ---------------- CUDA Timing ----------------

    cudaEvent_t start, stop;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Thread configuration
    dim3 threadsPerBlock(16, 16);

    dim3 blocksPerGrid(
        (N + threadsPerBlock.x - 1)
        / threadsPerBlock.x,

        (N + threadsPerBlock.y - 1)
        / threadsPerBlock.y
    );

    cudaEventRecord(start);

    // Kernel Launch
    matrixMultiply<<<blocksPerGrid,
                     threadsPerBlock>>>
                     (d_A,
                      d_B,
                      d_C,
                      N);

    cudaEventRecord(stop);

    cudaEventSynchronize(stop);

    float gpuTimeMS;

    cudaEventElapsedTime(&gpuTimeMS,
                         start,
                         stop);

    // Convert ms -> seconds
    double parallelTime =
        gpuTimeMS / 1000.0;

    // Copy result back
    cudaMemcpy(C_par,
               d_C,
               size,
               cudaMemcpyDeviceToHost);

    // ---------------- Output ----------------

    printf("\nResult Matrix (First 5x5 Elements):\n\n");

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            printf("%d ",
                   C_par[i][j]);
        }

        printf("\n");
    }

    // ---------------- Performance ----------------

    printf("\n========== PERFORMANCE ==========\n");

    printf("Sequential Time = %f seconds\n",
           sequentialTime);

    printf("Parallel CUDA Time = %f seconds\n",
           parallelTime);

    // Speedup
    if (parallelTime > 0)
    {
        printf("Speedup = %f\n",
               sequentialTime
               / parallelTime);
    }
    else
    {
        printf("Speedup = Infinity\n");
    }

    // ---------------- Free Memory ----------------

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return 0;
}


/*Commands- 
1st - nvcc matrix.cu -o matrix
2nd - ./matrix
*/