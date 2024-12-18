#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // MPI 환경 초기화
    MPI_Init(&argc, &argv);

    // 총 프로세스 수와 현재 프로세스 ID 가져오기
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // 총 프로세스 수
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // 현재 프로세스의 ID
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len); // 프로세서 이름 가져오기

    // 현재 프로세스에서 메시지 출력
    printf("Hello, World from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);

    // MPI 환경 정리
    MPI_Finalize();
    return 0;
}
