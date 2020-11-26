#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void	writeVector(char *file, size_t str, float *vector) {
	FILE *f;
	f = fopen(file, "wb");
	char typeF = 'f';

	fwrite(&typeF, sizeof(char), 1, f);
	fwrite(&str, sizeof(size_t), 1, f);
	for (int i = 0; i < str; i++)
		fwrite(&(vector[i]), sizeof(float), 1, f);
	fclose(f);
}

float	*readVector(char *file, size_t *str) {
	float	*vector;
	char	type;
	FILE	*f;
	f = fopen(file, "rb");
	fread(&type, sizeof(char), 1, f);

	fread(str, sizeof(size_t), 1, f);
	vector = (float*) malloc(sizeof(float*) * *str);
	for (int i = 0; i < *str; i++)
		fread(&(vector[i]), sizeof(float), 1, f);
	fclose(f);
	return vector;
}

float	**readMatrix(char *file, size_t *str, size_t *clm) {
	float	**matrix;
	char	type;
	FILE	*f;
	f = fopen(file, "rb");
	fread(&type, sizeof(char), 1, f);

	fread(str, sizeof(size_t), 1, f);
	fread(clm, sizeof(size_t), 1, f);
	matrix = (float**) malloc(sizeof(float*) * *str);
    for (int i = 0; i < *str; i++)
        matrix[i] = malloc(sizeof(float) * *clm);
	for (int i = 0; i < *str; i++)
		for (int j = 0; j < *clm; j++)
			fread(&(matrix[i][j]), sizeof(float), 1, f);
	fclose(f);
	return matrix;
}

int main(int argc, char **argv)
{
	int size, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;
	MPI_Barrier(MPI_COMM_WORLD);
	size_t str, clm;
	int countStr;
	if (!rank) {
		float **matrix = readMatrix(argv[1], &str, &clm);
		// вывод матрицы 
		for (int i = 0; i < str; i++) {
			for (int j = 0; j < clm; j++)
				printf("%f ", matrix[i][j]);
			printf("\n");
		}
		printf("\n");
		//
		float *vectorB = readVector(argv[2], &clm);
		// вывод вектора В
		for (int i = 0; i < clm; i++)
			printf("%f ", vectorB[i]);
		printf("\n\n");
		//
		countStr = str / size + (str % size != 0);
		int j = countStr;
		for (int i = 1; i < size; i++) {
			MPI_Send(&str, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&clm, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&countStr, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
			int end = j + countStr;
			for (; j < str && j < end; j++) {
				MPI_Send(matrix[j], clm, MPI_FLOAT, i, 3, MPI_COMM_WORLD);
			}
			MPI_Send(vectorB, clm, MPI_FLOAT, i, 4, MPI_COMM_WORLD);
		}
		float vectorC[str];
		for (int i = 0; i < str && i < countStr; i++) {
			vectorC[i] = 0;
			for (int j = 0; j < clm; j++)
				vectorC[i] += matrix[i][j] * vectorB[j];
		}
		for (int i = 1; i < size; i++) {
			float vectorCThreads[str];
			MPI_Recv(vectorCThreads, str, MPI_FLOAT, i, 5, MPI_COMM_WORLD, &status);
			for (int j = i * countStr; j < str && j < (i + 1) * countStr; j++)
				vectorC[j] = vectorCThreads[j];
		}
		// вывод вектора С
		for (int i = 0; i < str; i++)
			printf("%f ", vectorC[i]);
		printf("\n");
		//
		writeVector(argv[3], str, vectorC);
	} else {
		MPI_Recv(&str, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&clm, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&countStr, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
		float matrix[countStr][(int)clm];
		for (int i = 0; i < countStr; i++)
			MPI_Recv(matrix[i], (int) clm, MPI_FLOAT, 0, 3, MPI_COMM_WORLD, &status);
		float vectorB[(int)clm];
		MPI_Recv(vectorB, clm, MPI_FLOAT, 0, 4, MPI_COMM_WORLD, &status);
		float vectorC[(int)str];
		for (int i = rank * countStr; i < str && i < (rank + 1) * countStr; i++) {
			vectorC[i] = 0;
			for (int j = 0; j < (int) clm; j++)
				vectorC[i] += matrix[i - rank * countStr][j] * vectorB[j];
		}
		MPI_Send(vectorC, (int)str, MPI_FLOAT, 0, 5, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}
