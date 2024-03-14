/*
 * Copyright (c) Cornell University.
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: I-Hsuan (Ethan) Huang
 * Email: ih246@cornell.edu
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <omp.h> // Include the OpenMP header
#include <chrono>

using namespace std;
using namespace std::chrono;

// 1. Basic Matrix Multiplication
// Function to multiply two matrices
vector<vector<int>> multiplyMatrices(const vector<vector<int>> &matrix1, const vector<vector<int>> &matrix2)
{
    if (matrix1.empty() || matrix2.empty())
        return {};

    int rows1 = matrix1.size();
    int cols1 = matrix1[0].size();
    int rows2 = matrix2.size();
    int cols2 = matrix2[0].size();

    // Check if multiplication is possible
    if (cols1 != rows2)
    {
        throw invalid_argument("Matrices cannot be multiplied due to incompatible dimensions.");
    }

    // Initialize the result matrix with 0s
    vector<vector<int>> result(rows1, vector<int>(cols2, 0));

    // Perform multiplication
    for (int i = 0; i < rows1; i++)
    {
        for (int j = 0; j < cols2; j++)
        {
            for (int k = 0; k < cols1; k++)
            {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result;
}

// 2. Cache Friendly
// Function to multiply two matrices using loop tiling
vector<vector<int>> multiplyMatricesCacheFriendly(const vector<vector<int>> &matrix1, const vector<vector<int>> &matrix2)
{
    const int BLOCK_SIZE = 128; // Example block size, adjust based on system's cache
    if (matrix1.empty() || matrix2.empty())
        return {};

    int rows1 = matrix1.size();
    int cols1 = matrix1[0].size();
    int rows2 = matrix2.size();
    int cols2 = matrix2[0].size();

    // Check if multiplication is possible
    if (cols1 != rows2)
    {
        throw invalid_argument("Matrices cannot be multiplied due to incompatible dimensions.");
    }

    // Initialize the result matrix with 0s
    vector<vector<int>> result(rows1, vector<int>(cols2, 0));

    // Perform multiplication with loop tiling

    for (int k0 = 0; k0 < cols1; k0 += BLOCK_SIZE)
    {
        for (int i0 = 0; i0 < rows1; i0 += BLOCK_SIZE)
        {
            for (int j0 = 0; j0 < cols2; j0 += BLOCK_SIZE)
            {
                for (int k = k0; k < min(k0 + BLOCK_SIZE, cols1); ++k)
                {
                    for (int i = i0; i < min(i0 + BLOCK_SIZE, rows1); ++i)
                    {
                        for (int j = j0; j < min(j0 + BLOCK_SIZE, cols2); ++j)
                        {
                            result[i][j] += matrix1[i][k] * matrix2[k][j];
                        }
                    }
                }
            }
        }
    }

    return result;
}

// 3. Strassen's Algorithm
// Function to add two matrices
vector<vector<int>> add(const vector<vector<int>> &A, const vector<vector<int>> &B)
{
    int n = A.size();
    vector<vector<int>> C(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
    return C;
}

// Function to subtract two matrices
vector<vector<int>> subtract(const vector<vector<int>> &A, const vector<vector<int>> &B)
{
    int n = A.size();
    vector<vector<int>> C(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            C[i][j] = A[i][j] - B[i][j];
        }
    }
    return C;
}

// Strassen's algorithm for matrix multiplication
vector<vector<int>> strassenMultiply(const vector<vector<int>> &A, const vector<vector<int>> &B)
{
    int n = A.size();
    vector<vector<int>> C(n, vector<int>(n, 0));

    // Base case: 1x1 matrix
    if (n == 1)
    {
        C[0][0] = A[0][0] * B[0][0];
        return C;
    }

    // Divide matrices into quarters
    int k = n / 2;
    vector<vector<int>> A11(k, vector<int>(k)), A12(k, vector<int>(k)),
        A21(k, vector<int>(k)), A22(k, vector<int>(k)),
        B11(k, vector<int>(k)), B12(k, vector<int>(k)),
        B21(k, vector<int>(k)), B22(k, vector<int>(k));

    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + k];
            A21[i][j] = A[i + k][j];
            A22[i][j] = A[i + k][j + k];
            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + k];
            B21[i][j] = B[i + k][j];
            B22[i][j] = B[i + k][j + k];
        }
    }

    // Recursively calculate the 7 products using Strassen's formula
    auto P1 = strassenMultiply(A11, subtract(B12, B22));
    auto P2 = strassenMultiply(add(A11, A12), B22);
    auto P3 = strassenMultiply(add(A21, A22), B11);
    auto P4 = strassenMultiply(A22, subtract(B21, B11));
    auto P5 = strassenMultiply(add(A11, A22), add(B11, B22));
    auto P6 = strassenMultiply(subtract(A12, A22), add(B21, B22));
    auto P7 = strassenMultiply(subtract(A11, A21), add(B11, B12));

    // Calculate the final quarters of the result matrix
    auto C11 = add(subtract(add(P5, P4), P2), P6);
    auto C12 = add(P1, P2);
    auto C21 = add(P3, P4);
    auto C22 = subtract(subtract(add(P1, P5), P3), P7);

    // Combine the quarters into the final result matrix
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            C[i][j] = C11[i][j];
            C[i][j + k] = C12[i][j];
            C[i + k][j] = C21[i][j];
            C[i + k][j + k] = C22[i][j];
        }
    }

    return C;
}

// 4.  parallel basic matrix multiplication
// Function to multiply two matrices in parallel using OpenMP
vector<vector<int>> parallelMatrixMultiply(const vector<vector<int>> &matrix1, const vector<vector<int>> &matrix2)
{
    if (matrix1.empty() || matrix2.empty())
        return {};

    int rows1 = matrix1.size();
    int cols1 = matrix1[0].size();
    int rows2 = matrix2.size();
    int cols2 = matrix2[0].size();

    // Check if multiplication is possible
    if (cols1 != rows2)
    {
        throw invalid_argument("Matrices cannot be multiplied due to incompatible dimensions.");
    }

    vector<vector<int>> result(rows1, vector<int>(cols2, 0));

#pragma omp parallel for collapse(2) // Parallelize the outer two loops
    for (int i = 0; i < rows1; i++)
    {
        for (int k = 0; k < cols1; k++)
        {
            for (int j = 0; j < cols2; j++)
            {

#pragma omp atomic // Protects the write operation
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result;
}

// 5. Cannon's algorithm for parallel matrix multiplication
// Simple function to perform block-wise multiplication of two matrices
vector<vector<int>> blockWiseMultiply(const vector<vector<int>> &A, const vector<vector<int>> &B)
{
    const int BLOCK_SIZE = 64; // Adjust based on cache size
    int n = A.size();
    vector<vector<int>> C(n, vector<int>(n, 0));

#pragma omp parallel for collapse(2) shared(A, B, C)

    for (int k0 = 0; k0 < n; k0 += BLOCK_SIZE)
    {
        for (int i0 = 0; i0 < n; i0 += BLOCK_SIZE)
        {
            for (int j0 = 0; j0 < n; j0 += BLOCK_SIZE)
            {
                for (int i = i0; i < min(i0 + BLOCK_SIZE, n); ++i)
                {
                    for (int k = k0; k < min(k0 + BLOCK_SIZE, n); ++k)
                    {
                        for (int j = j0; j < min(j0 + BLOCK_SIZE, n); ++j)
                        {
                            C[i][j] += A[i][k] * B[k][j];
                        }
                    }
                }
            }
        }
    }

    return C;
}

// 6. matrix multiplication using the fork-join model in shared-memory parallelism
// Helper function to create a submatrix
vector<vector<int>> getSubmatrix(const vector<vector<int>> &matrix, int rowStart, int colStart, int size)
{
    vector<vector<int>> submatrix(size, vector<int>(size, 0));
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            submatrix[i][j] = matrix[rowStart + i][colStart + j];
        }
    }
    return submatrix;
}

// Helper function to add matrices A and B, store result in C
void addMatrices(const vector<vector<int>> &A, const vector<vector<int>> &B, vector<vector<int>> &C, int size)
{
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
}

// Recursive function to multiply matrices A and B, store result in C
void multiplyRecursive(const vector<vector<int>> &A, const vector<vector<int>> &B, vector<vector<int>> &C, int size)
{
    if (size == 1)
    {
        C[0][0] = A[0][0] * B[0][0];
    }
    else
    {
        int newSize = size / 2;
        vector<vector<int>> A11 = getSubmatrix(A, 0, 0, newSize);
        vector<vector<int>> A12 = getSubmatrix(A, 0, newSize, newSize);
        vector<vector<int>> A21 = getSubmatrix(A, newSize, 0, newSize);
        vector<vector<int>> A22 = getSubmatrix(A, newSize, newSize, newSize);
        vector<vector<int>> B11 = getSubmatrix(B, 0, 0, newSize);
        vector<vector<int>> B12 = getSubmatrix(B, 0, newSize, newSize);
        vector<vector<int>> B21 = getSubmatrix(B, newSize, 0, newSize);
        vector<vector<int>> B22 = getSubmatrix(B, newSize, newSize, newSize);

        vector<vector<int>> C11(newSize, vector<int>(newSize, 0));
        vector<vector<int>> C12(newSize, vector<int>(newSize, 0));
        vector<vector<int>> C21(newSize, vector<int>(newSize, 0));
        vector<vector<int>> C22(newSize, vector<int>(newSize, 0));
        vector<vector<int>> temp1(newSize, vector<int>(newSize, 0));
        vector<vector<int>> temp2(newSize, vector<int>(newSize, 0));

#pragma omp parallel sections
        {
#pragma omp section
            {
                multiplyRecursive(A11, B11, C11, newSize);
                multiplyRecursive(A12, B21, temp1, newSize);
                addMatrices(C11, temp1, C11, newSize);
            }
#pragma omp section
            {
                multiplyRecursive(A11, B12, C12, newSize);
                multiplyRecursive(A12, B22, temp2, newSize);
                addMatrices(C12, temp2, C12, newSize);
            }
#pragma omp section
            {
                multiplyRecursive(A21, B11, C21, newSize);
                multiplyRecursive(A22, B21, temp1, newSize);
                addMatrices(C21, temp1, C21, newSize);
            }
#pragma omp section
            {
                multiplyRecursive(A21, B12, C22, newSize);
                multiplyRecursive(A22, B22, temp2, newSize);
                addMatrices(C22, temp2, C22, newSize);
            }
        }

        // Combine the results into C
        for (int i = 0; i < newSize; ++i)
        {
            for (int j = 0; j < newSize; ++j)
            {
                C[i][j] = C11[i][j];
                C[i][j + newSize] = C12[i][j];
                C[i + newSize][j] = C21[i][j];
                C[i + newSize][j + newSize] = C22[i][j];
            }
        }
    }
}

vector<vector<int>> sharedMemoryMultiply(const vector<vector<int>> &A, const vector<vector<int>> &B)
{
    int n = A.size();                            // Assuming A and B are square and of equal size.
    vector<vector<int>> C(n, vector<int>(n, 0)); // Initialize the result matrix.
    multiplyRecursive(A, B, C, n);
    return C;
}

// Performance Test

// Function to print a matrix
void printMatrix(const vector<vector<int>> &matrix)
{
    for (const auto &row : matrix)
    {
        for (int val : row)
        {
            cout << val << " ";
        }
        cout << endl;
    }
}

// Function to generate a random matrix of given dimensions
vector<vector<int>> generateRandomMatrix(int rows, int cols)
{
    random_device rd;                       // Obtain a random number from hardware
    mt19937 eng(rd());                      // Seed the generator
    uniform_int_distribution<> distr(0, 9); // Define the range

    vector<vector<int>> matrix(rows, vector<int>(cols));
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            matrix[i][j] = distr(eng); // Assign random numbers
        }
    }
    return matrix;
}

// int main() {
//     // Smaller matrices for initial test
//     vector<vector<int>> matrix1 = {{1, 2, 3}, {4, 5, 6}};
//     vector<vector<int>> matrix2 = {{7, 8}, {9, 10}, {11, 12}};

//     // Larger matrices for performance test
//     int largeSize = 100; // Adjust this size based on your system's capability
//     vector<vector<int>> largeMatrix1 = generateRandomMatrix(largeSize, largeSize);
//     vector<vector<int>> largeMatrix2 = generateRandomMatrix(largeSize, largeSize);

//     try {
//         vector<vector<int>> result = multiplyMatricesCacheFriendly(matrix1, matrix2);
//         cout << "Result of small matrix multiplication:" << endl;
//         printMatrix(result);

//         // Uncomment the lines below to test with large matrices
//         cout << "Multiplying large matrices..." << endl;
//         vector<vector<int>> largeResult = multiplyMatricesCacheFriendly(largeMatrix1, largeMatrix2);
//         cout << "Result of Large matrix multiplication:" << endl;
//         printMatrix(largeResult);
//         cout << "Done multiplying large matrices. Not displaying due to size." << endl;
//     } catch (const exception& e) {
//         cerr << "Error: " << e.what() << endl;
//     }

//     return 0;
// }

// int main()
// {

//     for (int size = 256; size <= 2048; size += 256)
//     {
//         cout << "Matrix size: " << size << "x" << size << endl;

//         vector<vector<int>> A = generateRandomMatrix(size, size);
//         vector<vector<int>> B = generateRandomMatrix(size, size);

//         vector<pair<string, long long>> timings;

//         // Analyze each method
//         // 1. Basic Matrix Multiplication
//         // Memory Usage: High (stores full result matrix, no extra memory)
//         // Additional Operations: Minimal
//         // Cache Utilization: Low (straightforward access pattern may not be optimal for cache)
//         // auto start = high_resolution_clock::now();
//         // auto C1 = multiplyMatrices(A, B);
//         // auto stop = high_resolution_clock::now();
//         // timings.emplace_back("Basic: ", duration_cast<milliseconds>(stop - start).count());

//         // 2. Cache Friendly Matrix Multiplication
//         // Memory Usage: High (similar to basic, but with potential for additional stack usage for blocks)
//         // Additional Operations: Minimal
//         // Cache Utilization: High (designed to improve cache hit rate by accessing blocks that fit in cache)
//         auto start = high_resolution_clock::now();
//         auto C2 = multiplyMatricesCacheFriendly(A, B);
//         auto stop = high_resolution_clock::now();
//         timings.emplace_back("Cache Friendly: ", duration_cast<milliseconds>(stop - start).count());

//         // 3. Strassen's Algorithm
//         // Memory Usage: Very High (requires additional matrices for the 7 recursive multiplications)
//         // Additional Operations: Higher (involves additions and subtractions of matrices)
//         // Cache Utilization: Moderate (recursive nature can be more cache-friendly than basic method, but less so than block-wise methods)
//         // start = high_resolution_clock::now();
//         // auto C3 = strassenMultiply(A, B);
//         // stop = high_resolution_clock::now();
//         // timings.emplace_back("Strassen: ", duration_cast<milliseconds>(stop - start).count());

//         // 4. Parallel Matrix Multiplication
//         // Memory Usage: High (similar to basic method)
//         // Additional Operations: Increased (due to overhead of managing parallel threads)
//         // Cache Utilization: Depends on implementation details and hardware
//         start = high_resolution_clock::now();
//         auto C4 = parallelMatrixMultiply(A, B);
//         stop = high_resolution_clock::now();
//         timings.emplace_back("Parallel: ", duration_cast<milliseconds>(stop - start).count());

//         // 5. Block-wise (Cannon's style) Matrix Multiplication
//         // Memory Usage: High (similar to cache-friendly method)
//         // Additional Operations: Minimal
//         // Cache Utilization: High (optimized block-wise access pattern)
//         start = high_resolution_clock::now();
//         auto C5 = blockWiseMultiply(A, B);
//         stop = high_resolution_clock::now();
//         timings.emplace_back("Block-wise: ", duration_cast<milliseconds>(stop - start).count());

//         // 6. Shared-memory parallelism (Fork–join model) Matrix Multiplication
//         // Memory Usage: Very High (similar to cache-friendly method)
//         // Additional Operations: Higher
//         // Cache Utilization:  (optimized block-wise access pattern)
//         // start = high_resolution_clock::now();
//         // auto C6 = sharedMemoryMultiply(A, B);
//         // stop = high_resolution_clock::now();
//         // timings.emplace_back("Fork-join model in shared-memory: ", duration_cast<milliseconds>(stop - start).count());

//         // Sort and print timings
//         std::sort(timings.begin(), timings.end(), [](const pair<string, long long> &a, const pair<string, long long> &b)
//                   { return a.second < b.second; });

//         for (auto &timing : timings)
//         {
//             cout << "  " << timing.first << " Time: " << timing.second << " ms" << endl;
//         }

//         cout << endl;
//     }

//     return 0;
// }

// For python test
int main()
{

    for (int size = 200; size <= 2000; size += 200)
    {
        cout << "Matrix size: " << size << "x" << size << endl;

        vector<vector<int>> A = generateRandomMatrix(size, size);
        vector<vector<int>> B = generateRandomMatrix(size, size);

        // 1. Basic Matrix Multiplication
        auto start = high_resolution_clock::now();
        auto C1 = multiplyMatrices(A, B);
        auto stop = high_resolution_clock::now();
        cout << "Basic: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";

        // 2. Cache Friendly Matrix Multiplication
        start = high_resolution_clock::now();
        auto C2 = multiplyMatricesCacheFriendly(A, B);
        stop = high_resolution_clock::now();
        cout << "Cache Friendly: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";

        // 3. Strassen's Algorithm
        start = high_resolution_clock::now();
        auto C3 = strassenMultiply(A, B);
        stop = high_resolution_clock::now();
        cout << "Strassen: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";

        // 4. Parallel Matrix Multiplication
        start = high_resolution_clock::now();
        auto C4 = parallelMatrixMultiply(A, B);
        stop = high_resolution_clock::now();
        cout << "Parallel: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";

        // 5. Block-wise (Cannon's style) Matrix Multiplication
        start = high_resolution_clock::now();
        auto C5 = blockWiseMultiply(A, B);
        stop = high_resolution_clock::now();
        cout << "Block-wise: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";

        // 6. Shared-memory parallelism (Fork–join model)
        start = high_resolution_clock::now();
        auto C6 = sharedMemoryMultiply(A, B);
        stop = high_resolution_clock::now();
        cout << "Fork-join model in shared-memory: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";

        cout << endl;
    }

    return 0;
}

// g++ -fopenmp -march=native -ffast-math -fopt-info-vec -o MatrixMultiply MatrixMultiply.cpp -O3 && ./MatrixMultiply
// valgrind --tool=cachegrind ./MatrixMultiply


// ==325== Cachegrind, a cache and branch-prediction profiler
// ==325== Copyright (C) 2002-2017, and GNU GPL'd, by Nicholas Nethercote et al.
// ==325== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
// ==325== Command: ./MatrixMultiply
// ==325==
// --325-- warning: L3 cache found, using its data for the LL simulation.
// --325-- warning: specified LL cache: line_size 64  assoc 16  total_size 12,582,912
// --325-- warning: simulated LL cache: line_size 64  assoc 24  total_size 12,582,912
// Matrix size: 256x256
//   Cache Friendly:  Time: 321 ms
//   Parallel:  Time: 2844 ms
//   Block-wise:  Time: 2966 ms

// Matrix size: 512x512
//   Cache Friendly:  Time: 2697 ms
//   Parallel:  Time: 21804 ms
//   Block-wise:  Time: 23108 ms

// Matrix size: 768x768
// ==325== brk segment overflow in thread #1: can't grow to 0x4851000
// ==325== (see section Limitations in user manual)
// ==325== NOTE: further instances of this message will not be shown

//   Cache Friendly:  Time: 8836 ms
//   Parallel:  Time: 72713 ms
//   Block-wise:  Time: 78379 ms

// Matrix size: 1024x1024

// ^[[B^[[B^[[A^[[A  Cache Friendly:  Time: 21799 ms
//   Parallel:  Time: 181382 ms
//   Block-wise:  Time: 196793 ms

// Matrix size: 1280x1280

//   Cache Friendly:  Time: 41662 ms
//   Parallel:  Time: 346354 ms
//   Block-wise:  Time: 360539 ms

// Matrix size: 1536x1536
//   Cache Friendly:  Time: 72161 ms
//   Parallel:  Time: 619099 ms
//   Block-wise:  Time: 670887 ms

// Matrix size: 1792x1792
//   Cache Friendly:  Time: 124589 ms
//   Block-wise:  Time: 1003879 ms
//   Parallel:  Time: 1029731 ms

// Matrix size: 2048x2048
//   Cache Friendly:  Time: 182483 ms
//   Parallel:  Time: 1443399 ms
//   Block-wise:  Time: 1488131 ms

// ==325==
// ==325== I   refs:      818,519,398,743
// ==325== I1  misses:              4,587
// ==325== LLi misses:              3,769
// ==325== I1  miss rate:            0.00%
// ==325== LLi miss rate:            0.00%
// ==325==
// ==325== D   refs:      584,802,692,180  (579,033,122,983 rd   + 5,769,569,197 wr)
// ==325== D1  misses:      4,586,209,103  (  4,580,312,869 rd   +     5,896,234 wr)
// ==325== LLd misses:        699,332,675  (    694,918,386 rd   +     4,414,289 wr)
// ==325== D1  miss rate:             0.8% (            0.8%     +           0.1%  )
// ==325== LLd miss rate:             0.1% (            0.1%     +           0.1%  )
// ==325==
// ==325== LL refs:         4,586,213,690  (  4,580,317,456 rd   +     5,896,234 wr)
// ==325== LL misses:         699,336,444  (    694,922,155 rd   +     4,414,289 wr)
// ==325== LL miss rate:              0.0% (            0.0%     +           0.1%  )