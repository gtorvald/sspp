#include <stdio.h>
#include <stdlib.h>

void	writeMatrix(char *file, int N, int M) {
	FILE *f;
	f = fopen(file, "wb");
	char typeF = 'f';
	int i, j, N_local = N, M_local = M;

	fwrite(&typeF, sizeof(char), 1, f);
	fwrite(&N_local, sizeof(size_t), 1, f);
	fwrite(&M_local, sizeof(size_t), 1, f);
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			float num = ((float)rand()/(float)(RAND_MAX));
			fwrite(&num, sizeof(float), 1, f);
			printf("%f ", num);
		}
		printf("\n");
	}
	fclose(f);
}

void	writeVector(char *file, int M) {
	FILE *f;
	f = fopen(file, "wb");
	char typeF = 'f';
	int i;

	fwrite(&typeF, sizeof(char), 1, f);
	fwrite(&M, sizeof(size_t), 1, f);
	for (i = 0; i < M; i++) {
		float num = ((float)rand()/(float)(RAND_MAX));
		fwrite(&num, sizeof(float), 1, f);
		printf("%f ", num);
	}
	printf("\n");
	fclose(f);
}

int main(int argc, char **argv) {
	int N = atoi(argv[1]), M = atoi(argv[2]);
	writeMatrix(argv[3], N, M);
	writeVector(argv[4], M);
	return 0;
}
