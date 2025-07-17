#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int is_sorted(int arr[], int n) {
    for (int i = 0; i < n-1; i++)
        if (arr[i] > arr[i+1]) return 0;
    return 1;
}

void print_array(int *arr, int n, int rank) {
    if (rank == 0) {
        for (int i = 0; i < (n < 20 ? n : 20); i++) {
            printf("%d ", arr[i]);
        }
        printf(n > 20 ? "...\n" : "\n");
    }
}

void local_odd_even_sort(int *local, int local_n) {
    for (int phase = 0; phase < local_n; phase++) {
        if (phase % 2 == 0) {
            for (int i = 1; i < local_n; i += 2) {
                if (local[i-1] > local[i]) {
                    swap(&local[i-1], &local[i]);
                }
            }
        } else {
            for (int i = 1; i < local_n - 1; i += 2) {
                if (local[i] > local[i+1]) {
                    swap(&local[i], &local[i+1]);
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size, n, local_n;
    int *global_arr = NULL;
    int *local_arr = NULL;
    double comm_time = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) printf("Uso: %s <tamanho_array>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    n = atoi(argv[1]);
    local_n = n / size;

    local_arr = malloc(local_n * sizeof(int));

    if (rank == 0) {
        global_arr = malloc(n * sizeof(int));
        srand(time(NULL));
        for (int i = 0; i < n; i++) {
            global_arr[i] = rand() % 1000;
        }
        printf("Array original:\n");
        print_array(global_arr, n, rank);
    }

    MPI_Scatter(global_arr, local_n, MPI_INT, local_arr, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double total_start = MPI_Wtime();

    for (int phase = 0; phase < n; phase++) {
        local_odd_even_sort(local_arr, local_n);

        int partner = -1;
        if (phase % 2 == 0) {
            if (rank % 2 == 0 && rank < size - 1) partner = rank + 1;
            else if (rank % 2 == 1) partner = rank - 1;
        } else {
            if (rank % 2 == 1 && rank < size - 1) partner = rank + 1;
            else if (rank % 2 == 0) partner = rank - 1;
        }

        if (partner >= 0 && partner < size) {
            int send_val = (rank < partner) ? local_arr[local_n - 1] : local_arr[0];
            int recv_val;
            double comm_start = MPI_Wtime();

            MPI_Sendrecv(&send_val, 1, MPI_INT, partner, 0,
                         &recv_val, 1, MPI_INT, partner, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            double comm_end = MPI_Wtime();
            comm_time += (comm_end - comm_start);

            if ((rank < partner && send_val > recv_val) || 
                (rank > partner && send_val < recv_val)) {
                if (rank < partner) local_arr[local_n - 1] = recv_val;
                else local_arr[0] = recv_val;
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double total_end = MPI_Wtime();
    double total_time = total_end - total_start;

    double total_time_global, comm_time_global;
    MPI_Reduce(&total_time, &total_time_global, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&comm_time, &comm_time_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Gather(local_arr, local_n, MPI_INT, global_arr, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\nArray ordenado:\n");
        print_array(global_arr, n, rank);
        printf("Array está ordenado: %s\n", is_sorted(global_arr, n) ? "Sim" : "Não");
        printf("Tempo Total: %.6f s\n", total_time_global);
        printf("Tempo Comunicação (soma): %.6f s\n", comm_time_global);
        printf("Overhead estimado: %.2f%%\n", (comm_time_global / total_time_global) * 100);
        free(global_arr);
    }

    free(local_arr);
    MPI_Finalize();
    return 0;
}
