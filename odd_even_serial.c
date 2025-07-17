#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void generate_array(int *arr, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 1000;
    }
}

int is_sorted(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) return 0;
    }
    return 1;
}

void print_array(int *arr, int n) {
    int limit = (n < 20) ? n : 20;
    for (int i = 0; i < limit; i++) {
        printf("%d ", arr[i]);
    }
    if (n > 20) printf("...");
    printf("\n");
}

void odd_even_sort(int *arr, int n) {
    for (int phase = 0; phase < n; phase++) {
        if (phase % 2 == 0) {
            for (int i = 1; i < n; i += 2) {
                if (arr[i - 1] > arr[i]) {
                    swap(&arr[i - 1], &arr[i]);
                }
            }
        } else {
            for (int i = 1; i < n - 1; i += 2) {
                if (arr[i] > arr[i + 1]) {
                    swap(&arr[i], &arr[i + 1]);
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <tamanho_array>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int *arr = malloc(n * sizeof(int));

    generate_array(arr, n);

    printf("Array original:\n");
    print_array(arr, n);

    clock_t start = clock();
    odd_even_sort(arr, n);
    clock_t end = clock();

    double tempo = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\nArray ordenado:\n");
    print_array(arr, n);
    printf("Array está ordenado? %s\n", is_sorted(arr, n) ? "Sim" : "Não");
    printf("Tempo: %.6f s\n", tempo);

    free(arr);
    return 0;
}
