#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void generate_random_array(int arr[], int n, int max_val) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % max_val;
    }
}

int is_sorted(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) return 0;
    }
    return 1;
}

void odd_even_sort_parallel(int arr[], int n, int num_threads, omp_sched_t schedule_type) {
    int phase, i;

    // Define a política de escalonamento
    omp_set_schedule(schedule_type, 0);

    #pragma omp parallel num_threads(num_threads) private(phase, i) shared(arr, n)
    {
        for (phase = 0; phase < n; phase++) {
            if (phase % 2 == 0) {
                #pragma omp for schedule(runtime)
                for (i = 1; i < n; i += 2) {
                    if (arr[i - 1] > arr[i]) {
                        swap(&arr[i - 1], &arr[i]);
                    }
                }
            } else {
                #pragma omp for schedule(runtime)
                for (i = 1; i < n - 1; i += 2) {
                    if (arr[i] > arr[i + 1]) {
                        swap(&arr[i], &arr[i + 1]);
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <tamanho_array> <num_threads>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    int *arr = malloc(n * sizeof(int));
    generate_random_array(arr, n, 1000);

    printf("Array original: ");
    if (n <= 20) print_array(arr, n);
    else { print_array(arr, 20); printf("(exibindo apenas os 20 primeiros)\n"); }

    double start_time, end_time;

    // Testar diferentes políticas
    const char* schedules[] = {"static", "dynamic", "guided"};
    omp_sched_t sched_ids[] = {omp_sched_static, omp_sched_dynamic, omp_sched_guided};

    for (int s = 0; s < 3; s++) {
        generate_random_array(arr, n, 1000); // Reinicializa array
        omp_set_schedule(sched_ids[s], 0);   // Define política atual
        start_time = omp_get_wtime();
        odd_even_sort_parallel(arr, n, num_threads, sched_ids[s]);
        end_time = omp_get_wtime();

        printf("\n[Política %s] Tempo: %.6f segundos\n", schedules[s], end_time - start_time);
        printf("Array ordenado: ");
        if (n <= 20) print_array(arr, n);
        else { print_array(arr, 20); printf("(exibindo apenas os 20 primeiros)\n"); }
        printf("Array está ordenado: %s\n", is_sorted(arr, n) ? "Sim" : "Não");
    }

    free(arr);
    return 0;
}
