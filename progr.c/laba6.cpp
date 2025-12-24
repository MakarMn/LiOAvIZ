#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <locale.h>

using namespace std;


int** generate_adjacency_matrix(int n) {
    int** matrix = new int* [n];
    for (int i = 0; i < n; i++) {
        matrix[i] = new int[n];
    }


    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            if (i == j) {
                matrix[i][j] = 0;
            }
            else {
                matrix[i][j] = rand() % 2;
                matrix[j][i] = matrix[i][j];
            }
        }
    }

    return matrix;
}

void print_matrix(int** matrix, int n) {
    cout << "  ";
    for (int i = 0; i < n; i++) {
        cout << i + 1 << " ";
    }
    cout << endl;

    for (int i = 0; i < n; i++) {
        cout << i + 1 << " ";
        for (int j = 0; j < n; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void free_matrix(int** matrix, int n) {
    for (int i = 0; i < n; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

// Задание 2а: Отождествление вершин
int** identify_vertices(int** adj, int n, int u, int v, int& new_n) {
    new_n = n - 1;
    int** new_adj = new int* [new_n];
    for (int i = 0; i < new_n; i++) {
        new_adj[i] = new int[new_n]();
    }

    int* index_map = new int[n];
    int new_idx = 0;

    for (int i = 0; i < n; i++) {
        if (i != u && i != v) {
            index_map[i] = new_idx++;
        }
    }

    index_map[u] = new_n - 1;
    index_map[v] = new_n - 1;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int new_i = index_map[i];
            int new_j = index_map[j];

            if (adj[i][j] == 1) {
                new_adj[new_i][new_j] = 1;
            }
        }
    }

    for (int i = 0; i < new_n; i++) {
        new_adj[i][i] = 0;
    }

    delete[] index_map;
    return new_adj;
}

// Задание 2б: Стягивание ребра
int** contract_edge(int** adj, int n, int u, int v, int& new_n) {
    // Проверяем, существует ли ребро
    if (adj[u][v] == 0) {
        cout << "Ребро между вершинами " << u + 1 << " и " << v + 1 << " отсутствует!" << endl;
        new_n = n;
        int** copy = new int* [n];
        for (int i = 0; i < n; i++) {
            copy[i] = new int[n];
            for (int j = 0; j < n; j++) {
                copy[i][j] = adj[i][j];
            }
        }
        return copy;
    }

    return identify_vertices(adj, n, u, v, new_n);
}

int** split_vertex(int** adj, int n, int u, int& new_n) {
    new_n = n + 1;
    int** new_adj = new int* [new_n];
    for (int i = 0; i < new_n; i++) {
        new_adj[i] = new int[new_n]();
    }

    int new_vertex = n;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            new_adj[i][j] = adj[i][j];
        }
    }

    new_adj[u][new_vertex] = 1;
    new_adj[new_vertex][u] = 1;

    for (int i = 0; i < n; i++) {
        if (adj[u][i] == 1 && i != u) {

            if (rand() % 2 == 0) {

                new_adj[u][i] = 1;
                new_adj[i][u] = 1;

                new_adj[new_vertex][i] = 0;
                new_adj[i][new_vertex] = 0;
            }
            else {

                new_adj[u][i] = 0;
                new_adj[i][u] = 0;
                new_adj[new_vertex][i] = 1;
                new_adj[i][new_vertex] = 1;
            }
        }
    }

    return new_adj;
}

// Задание 3а: Объединение графов
int** union_graphs(int** adj1, int** adj2, int n) {
    int** result = new int* [n];
    for (int i = 0; i < n; i++) {
        result[i] = new int[n];
        for (int j = 0; j < n; j++) {
            result[i][j] = (adj1[i][j] || adj2[i][j]) ? 1 : 0;
        }
    }
    return result;
}

// Задание 3б: Пересечение графов
int** intersect_graphs(int** adj1, int** adj2, int n) {
    int** result = new int* [n];
    for (int i = 0; i < n; i++) {
        result[i] = new int[n];
        for (int j = 0; j < n; j++) {
            result[i][j] = (adj1[i][j] && adj2[i][j]) ? 1 : 0;
        }
    }
    return result;
}

// Задание 3в: Кольцевая сумма графов
int** ring_sum_graphs(int** adj1, int** adj2, int n) {
    int** result = new int* [n];
    for (int i = 0; i < n; i++) {
        result[i] = new int[n];
        for (int j = 0; j < n; j++) {
            result[i][j] = (adj1[i][j] != adj2[i][j]) ? 1 : 0;
        }
    }
    return result;
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(NULL));

    int n;
    cout << "Введите количество вершин графа: ";
    cin >> n;

    // Задание 1
    cout << "\n=== ЗАДАНИЕ 1 ===" << endl;

    int** M1 = generate_adjacency_matrix(n);
    int** M2 = generate_adjacency_matrix(n);

    cout << "\nМатрица смежности M1 (граф G1):" << endl;
    print_matrix(M1, n);

    cout << "Матрица смежности M2 (граф G2):" << endl;
    print_matrix(M2, n);

    // Задание 2
    cout << "\n=== ЗАДАНИЕ 2 ===" << endl;

    // а) Отождествление вершин
    cout << "\nа) Отождествление вершин" << endl;
    int u, v;
    cout << "Для графа G1 введите номера двух вершин для отождествления (1-" << n << "): ";
    cin >> u >> v;

    int new_n1;
    int** identify_result = identify_vertices(M1, n, u - 1, v - 1, new_n1);
    cout << "Результат отождествления вершин " << u << " и " << v << " в G1:" << endl;
    print_matrix(identify_result, new_n1);

    // б) Стягивание ребра
    cout << "\nб) Стягивание ребра" << endl;
    cout << "Для графа G1 введите номера двух вершин для стягивания ребра (1-" << n << "): ";
    cin >> u >> v;

    int new_n2;
    int** contract_result = contract_edge(M1, n, u - 1, v - 1, new_n2);
    cout << "Результат стягивания ребра между " << u << " и " << v << " в G1:" << endl;
    print_matrix(contract_result, new_n2);

    // в) Расщепление вершины
    cout << "\nв) Расщепление вершины" << endl;
    cout << "Для графа G1 введите номер вершины для расщепления (1-" << n << "): ";
    cin >> u;

    int new_n3;
    int** split_result = split_vertex(M1, n, u - 1, new_n3);
    cout << "Результат расщепления вершины " << u << " в G1:" << endl;
    print_matrix(split_result, new_n3);

    // Задание 3
    cout << "\n=== ЗАДАНИЕ 3 ===" << endl;

    // а) Объединение
    cout << "\nа) Объединение G1 ∪ G2:" << endl;
    int** union_result = union_graphs(M1, M2, n);
    print_matrix(union_result, n);

    // б) Пересечение
    cout << "\nб) Пересечение G1 ∩ G2:" << endl;
    int** intersect_result = intersect_graphs(M1, M2, n);
    print_matrix(intersect_result, n);

    // в) Кольцевая сумма
    cout << "\nв) Кольцевая сумма G1 ⊕ G2:" << endl;
    int** ring_sum_result = ring_sum_graphs(M1, M2, n);
    print_matrix(ring_sum_result, n);

    free_matrix(M1, n);
    free_matrix(M2, n);
    free_matrix(identify_result, new_n1);
    free_matrix(contract_result, new_n2);
    free_matrix(split_result, new_n3);
    free_matrix(union_result, n);
    free_matrix(intersect_result, n);
    free_matrix(ring_sum_result, n);

    return 0;
}