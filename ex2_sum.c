#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int local_sum = 0;    // 각 프로세스의 부분합
    int global_sum = 0;   // 전체 합
    int start, end;       // 각 프로세스가 처리할 시작과 끝 값
    double start_time, end_time; // 시간 측정을 위한 변수

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 프로세스당 할당되는 범위 계산
    int elements_per_proc = 100 / size; // 프로세스당 할당되는 값의 개수
    start = rank * elements_per_proc + 1;
    end = (rank == size - 1) ? 100 : start + elements_per_proc - 1;

    // rank 0에서 시작 시간 기록
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    // 각 프로세스에서 자신의 부분합 계산
    for (int i = start; i <= end; i++) {
        local_sum += i;
    }

    printf("Rank %d: Calculated local sum = %d (Range: %d to %d)\n", rank, local_sum, start, end);

    // 모든 프로세스의 부분합을 rank 0으로 모아 전체 합 계산
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // rank 0에서 결과 출력 및 종료 시간 기록
    if (rank == 0) {
        end_time = MPI_Wtime();
        printf("Global sum = %d\n", global_sum);
        printf("Elapsed time = %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
