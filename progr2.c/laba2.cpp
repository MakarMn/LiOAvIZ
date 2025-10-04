#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <math.h>

int** create_matrix(int n) {
    int** matrix = (int**)malloc(n * sizeof(int*));
    if (matrix == NULL) return NULL;

    for (int i = 0; i < n; i++) {
        matrix[i] = (int*)malloc(n * sizeof(int));
        if (matrix[i] == NULL) {

            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}


void free_matrix(int** matrix, int n) {
    if (matrix == NULL) return;

    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}


void fill_matrix(int** matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % 100 + 1;
        }
    }
}


double multiply_matrices(int n) {
    int** a = create_matrix(n);
    int** b = create_matrix(n);
    int** c = create_matrix(n);

    if (a == NULL || b == NULL || c == NULL) {
        printf("Ошибка выделения памяти для размера %d\n", n);
        if (a) free_matrix(a, n);
        if (b) free_matrix(b, n);
        if (c) free_matrix(c, n);
        return -1.0;
    }

    fill_matrix(a, n);
    fill_matrix(b, n);

    clock_t start = clock();


    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int elem_c = 0;
            for (int r = 0; r < n; r++) {
                elem_c += a[i][r] * b[r][j];
            }
            c[i][j] = elem_c;
        }
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    free_matrix(a, n);
    free_matrix(b, n);
    free_matrix(c, n);

    return time_taken;
}

int main(void) {
    setlocale(LC_ALL, "rus");
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    srand(time(NULL));

    int sizes[] = { 100, 200, 400, 1000, 2000, 4000, 10000 };
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);


    FILE* file = fopen("matrix_multiply_results.txt", "w");
    if (file == NULL) {
        printf("Ошибка открытия файла!\n");
        return 1;
    }

    printf("Размер матрицы | Время умножения (сек) | \n");
    printf("------------------------------------------------------------\n");

    fprintf(file, "Размер матрицы\tВремя умножения (сек)\\n");

    for (int idx = 0; idx < num_sizes; idx++) {
        int n = sizes[idx];

        printf("Обрабатывается размер %d...\n", n);

        double mult_time = multiply_matrices(n);

        if (mult_time < 0) {
            printf("Пропуск размера %d из-за ошибки памяти\n", n);
            continue;
        }

        double theoretical = pow(n, 3) / pow(100, 3);

        printf("%13d | %21.6f | %15.2f\n", n, mult_time, theoretical);
        fprintf(file, "%d\t%.6f\t%.2f\n", n, mult_time, theoretical);

        printf("Завершено: %dx%d - время: %.6f сек.\n", n, n, mult_time);
    }

    fclose(file);
    printf("\nРезультаты записаны в файл matrix_multiply_results.txt\n");

    return 0;
}