#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_SIZE 1000

// Function to initialize a matrix with random values
void initialize_matrix(double* matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = rand() % 10; // Random numbers between 0 and 9
    }
}

// Function to print a matrix (optional, for debugging purposes)
void print_matrix(double* matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%f ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_process = MATRIX_SIZE / size; // Number of rows per process

    // Allocate memory for matrices
    double* A = NULL;
    double* B = NULL;
    double* C = (double*)malloc(rows_per_process * MATRIX_SIZE * sizeof(double)); // Result matrix for each process

    if (rank == 0) {
        // Only the root process initializes the matrices
        A = (double*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
        B = (double*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
        initialize_matrix(A, MATRIX_SIZE, MATRIX_SIZE);
        initialize_matrix(B, MATRIX_SIZE, MATRIX_SIZE);
    }

    // Scatter rows of matrix A to all processes
    double* A_sub = (double*)malloc(rows_per_process * MATRIX_SIZE * sizeof(double));
    MPI_Scatter(A, rows_per_process * MATRIX_SIZE, MPI_DOUBLE, A_sub, rows_per_process * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Broadcast matrix B to all processes
    if (rank == 0) {
        MPI_Bcast(B, MATRIX_SIZE * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    } else {
        B = (double*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
        MPI_Bcast(B, MATRIX_SIZE * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // Start the timer
    double start_time = MPI_Wtime();

    // Perform matrix multiplication for the assigned rows
    for (int i = 0; i < rows_per_process; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            C[i * MATRIX_SIZE + j] = 0.0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                C[i * MATRIX_SIZE + j] += A_sub[i * MATRIX_SIZE + k] * B[k * MATRIX_SIZE + j];
            }
        }
    }

    // Gather the results from all processes
    if (rank == 0) {
        MPI_Gather(MPI_IN_PLACE, rows_per_process * MATRIX_SIZE, MPI_DOUBLE, C, rows_per_process * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    } else {
        MPI_Gather(C, rows_per_process * MATRIX_SIZE, MPI_DOUBLE, NULL, rows_per_process * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // Stop the timer
    double end_time = MPI_Wtime();

    // Root process prints the elapsed time
    if (rank == 0) {
        printf("Matrix multiplication completed in %f seconds.\n", end_time - start_time);

        // Uncomment to print the resulting matrix
        // print_matrix(C, MATRIX_SIZE, MATRIX_SIZE);

        // Free matrices on the root process
        free(A);
        free(B);
    }

    // Free allocated memory
    free(A_sub);
    free(C);
    if (rank != 0) {
        free(B);
    }

    MPI_Finalize();
    return 0;
}
