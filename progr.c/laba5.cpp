#define _CRT_SECURE_NO_WAARNINGS

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <clocale>

int main() {
    setlocale(LC_ALL, "rus");
    srand(time(0)); 

    int** G;
    int* loob, * deg;
    int N, size = 0;

    printf("Введите размер матрицы N: ");
    scanf_s("%d", &N);

    G = (int**)malloc(sizeof(int*) * N);

    for (int i = 0; i < N; i++) {
        G[i] = (int*)malloc(N * sizeof(int));
    }

    loob = (int*)malloc(N * sizeof(int));
    deg = (int*)malloc(N * sizeof(int));

    for (int i = 0; i < N; i++) {
        loob[i] = 0;
        deg[i] = 0;
    }

    printf("Матрица смежности:\n");
    for (int i = 0; i < N; i++) {
        for (int j = i; j < N; j++) {
            if (i == j) {
                G[i][j] = rand() % 2; 
            }
            else {
                G[i][j] = G[j][i] = rand() % 2; 
            }
            size += G[i][j]; 
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", G[i][j]);
        }
        printf("\n");
    }

    printf("Размер графа (количество рёбер): %d\n", size);

    for (int i = 0; i < N; i++) {
        loob[i] = G[i][i]; 
        for (int j = 0; j < N; j++) {
            deg[i] += G[i][j]; 
        }
    }

    printf("\nАнализ вершин:\n");
    for (int i = 0; i < N; i++) {
        printf("Вершина %d: степень = %d, петля = %d\n", i, deg[i], loob[i]);

        if (loob[i] == 0 && deg[i] == 0) {
            printf("  - Изолированная вершина: %d\n", i);
        }

        if (deg[i] == 1 && loob[i] == 0) {
            printf("  - Концевая вершина: %d\n", i);
        }

        if ((deg[i] - 2 * loob[i]) == (N - 1)) {
            printf("  - Доминирующая вершина: %d\n", i);
        }
    }

    for (int i = 0; i < N; i++) {
        free(G[i]);
    }
    free(G);
    free(loob);
    free(deg);

    return 0;
}