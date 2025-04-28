#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PARTICLES 1000
#define NUM_STEPS 100
#define DIMENSIONS 3

typedef struct {
    double position[DIMENSIONS];
    double velocity[DIMENSIONS];
} Particle;

void initialize_particles(Particle *particles, int num_particles) {
    srand(time(NULL));
    for (int i = 0; i < num_particles; i++) {
        for (int d = 0; d < DIMENSIONS; d++) {
            particles[i].position[d] = (double)rand() / RAND_MAX * 100.0;
            particles[i].velocity[d] = (double)rand() / RAND_MAX * 10.0;
        }
    }
}

void update_particles(Particle *particles, int num_particles) {
    for (int i = 0; i < num_particles; i++) {
        for (int d = 0; d < DIMENSIONS; d++) {
            particles[i].position[d] += particles[i].velocity[d];
        }
    }
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int particles_per_proc = NUM_PARTICLES / size;
    Particle *particles = NULL;
    Particle *local_particles = malloc(particles_per_proc * sizeof(Particle));

    if (rank == 0) {
        particles = malloc(NUM_PARTICLES * sizeof(Particle));
        initialize_particles(particles, NUM_PARTICLES);
    }

    MPI_Scatter(particles, particles_per_proc * sizeof(Particle), MPI_BYTE,
                local_particles, particles_per_proc * sizeof(Particle), MPI_BYTE,
                0, MPI_COMM_WORLD);

    for (int step = 0; step < NUM_STEPS; step++) {
        update_particles(local_particles, particles_per_proc);

        MPI_Gather(local_particles, particles_per_proc * sizeof(Particle), MPI_BYTE,
                   particles, particles_per_proc * sizeof(Particle), MPI_BYTE,
                   0, MPI_COMM_WORLD);

        if (rank == 0) {
            printf("Step %d completed\n", step + 1);
        }
    }

    free(local_particles);
    if (rank == 0) {
        free(particles);
    }

    MPI_Finalize();
    return 0;
}