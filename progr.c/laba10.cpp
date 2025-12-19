#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <limits.h>
#include <queue>

using namespace std;

void printM(int** Matr, int numG) {
	for (int i = 0; i < numG; i++) {
		for (int j = 0; j < numG; j++) {
			printf("%3d", Matr[i][j]);
		}
		printf("\n");
	}
}

void BFSD(int** G, int numG, int** GD, int s) {
	queue<int> q;
	int v;
	int* dist = (int*)malloc(numG * sizeof(int));

	for (int i = 0; i < numG; i++) {
		dist[i] = INT_MAX;
	}

	dist[s] = 0;
	q.push(s);

	while (!q.empty()) {

		v = q.front();
		q.pop();

		for (int i = 0; i < numG; i++) {
			if (G[v][i] > 0 && dist[i] > dist[v] + G[v][i]) {
				q.push(i);
				dist[i] = dist[v] + 1;
			}
		}
	}
	for (int i = 0; i < numG; i++) {
		GD[s][i] = (dist[i] == INT_MAX ? -1 : dist[i]);
	}

	free(dist);
}

int main() {

	int** G;
	int** GD;
	int* ecc;
	int numG, current;

	printf("Input number of vertission : ");
	scanf("%d", &numG);

	ecc = (int*)malloc(numG * sizeof(int));
	G = (int**)malloc(numG * sizeof(int*));
	GD = (int**)malloc(numG * sizeof(int*));

	for (int i = 0; i < numG; i++) {
		G[i] = (int*)malloc(numG * sizeof(int));
		GD[i] = (int*)malloc(numG * sizeof(int));
	}

	for (int i = 0; i < numG; i++) {
		ecc[i] = -1;
		for (int j = i; j < numG; j++) {
			G[i][j] = G[j][i] = (i == j ? 0 : (rand() % 2) ? rand() % 11 : 0);
			GD[i][j] = GD[j][i] = 0;
		}
	}

	for (int i = 0; i < numG; i++) {
		for (int j = 0; j < numG; j++) {
			printf("%3d", G[i][j]);
		}

		printf("\n");
	}

	for (int i = 0; i < numG; i++) {
		BFSD(G, numG, GD, i);
	}
	printf("\n\n");


	printf("Distance matrix: \n");
	printM(GD, numG);
	printf("\n");
	printf("Vector eccentristy: \n");
	for (int i = 0; i < numG; i++) {
		for (int j = 0; j < numG; j++) {
			ecc[i] = (ecc[i] < GD[i][j] ? GD[i][j] : ecc[i]);
		}
		printf("%4d", ecc[i]);
	}


	for (int i = 0; i < numG; i++) {
		free(G[i]);
	}



	free(G);
	free(ecc);

	getchar();
	getchar();

	return 0;
}

