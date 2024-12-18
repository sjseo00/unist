#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000 // 행렬 크기 (1000x1000)

// 두 행렬의 초기화
void initialize_matrices(double A[N][N], double B[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = (double)(rand() % 10); // 랜덤 값 (0~9)
            B[i][j] = (double)(rand() % 10);
        }
    }
}

// 결과 행렬 출력 (디버깅용, 큰 행렬은 생략 가능)
void print_matrix(double C[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%.1f ", C[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    double A[N][N], B[N][N], C[N][N];
    double start_time, end_time;

    // MPI 초기화
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 각 프로세스가 처리할 행 수
    int rows_per_proc = N / size;
    int start_row = rank * rows_per_proc;
    int end_row = (rank == size - 1) ? N : start_row + rows_per_proc;

    // 모든 프로세스에서 결과 행렬을 0으로 초기화
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0.0;
        }
    }

    // 0번 프로세스에서 행렬 초기화
    if (rank == 0) {
        srand(time(NULL));
        initialize_matrices(A, B);
    }

    // 모든 프로세스에 B 전송 (B는 모든 프로세스가 공유해야 함)
    MPI_Bcast(B, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // A의 각 행을 프로세스에 나누어 전송
    double local_A[rows_per_proc][N];
    MPI_Scatter(A, rows_per_proc * N, MPI_DOUBLE, local_A, rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // 시작 시간 기록
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    // 각 프로세스가 자신의 범위에 해당하는 행렬 곱셈 수행
    double local_C[rows_per_proc][N];
    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < N; j++) {
            local_C[i][j] = 0.0;
            for (int k = 0; k < N; k++) {
                local_C[i][j] += local_A[i][k] * B[k][j];
            }
        }
    }

    // 모든 프로세스의 계산 결과를 C로 모음
    MPI_Gather(local_C, rows_per_proc * N, MPI_DOUBLE, C, rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // 종료 시간 기록
    if (rank == 0) {
        end_time = MPI_Wtime();
        printf("Elapsed time = %f seconds\n", end_time - start_time);

        // (디버깅용) 결과 행렬 출력
        // printf("Result matrix C:\n");
        // print_matrix(C);
    }

    // MPI 종료
    MPI_Finalize();
    return 0;
}
