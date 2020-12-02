#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void	writeVector(char *file, size_t str, float *vector) {
	FILE *f;
	f = fopen(file, "wb");
	char typeF = 'f';
	int i;

	fwrite(&typeF, sizeof(char), 1, f);
	fwrite(&str, sizeof(size_t), 1, f);
	for (i = 0; i < str; i++)
		fwrite(&(vector[i]), sizeof(float), 1, f);
	fclose(f);
}

float	*readVector(char *file, size_t *str) {
	float	*vector;
	char	type;
	FILE	*f;
	f = fopen(file, "rb");
	fread(&type, sizeof(char), 1, f);
	int i;

	fread(str, sizeof(size_t), 1, f);
	vector = (float*) malloc(sizeof(float*) * *str);
	for (i = 0; i < *str; i++)
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
	int i, j;

	fread(str, sizeof(size_t), 1, f);
	fread(clm, sizeof(size_t), 1, f);
	matrix = (float**) malloc(sizeof(float*) * *str);
    for (i = 0; i < *str; i++)
        matrix[i] = malloc(sizeof(float) * *clm);
	for (i = 0; i < *str; i++)
		for (j = 0; j < *clm; j++)
			fread(&(matrix[i][j]), sizeof(float), 1, f);
	fclose(f);
	return matrix;
}

int main(int argc, char **argv)
{
	int size, rank;
	int i, j;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;
	MPI_Barrier(MPI_COMM_WORLD);
	size_t str, clm;
	int countStr, countClm;
	double time_start, time_end;
	if (!rank) {
		float **matrix = readMatrix(argv[1], &str, &clm);
		// вывод матрицы
		// for (i = 0; i < str; i++) {
		// 	for (j = 0; j < clm; j++)
		// 		printf("%f ", matrix[i][j]);
		// 	printf("\n");
		// }
		// printf("\n");
		//
		float *vectorB = readVector(argv[2], &clm);
		// вывод вектора В
		// for (i = 0; i < clm; i++)
		// 	printf("%f ", vectorB[i]);
		// printf("\n\n");
		//
		float vectorC[str];
		if (str >= clm) {
			countStr = str / size + (str % size != 0);
			int j = countStr;
			for (i = 1; i < size; i++) {
				MPI_Send(&str, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&clm, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
				MPI_Send(&countStr, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
				int end = j + countStr;
				for (; j < str && j < end; j++) {
					MPI_Send(matrix[j], clm, MPI_FLOAT, i, 3, MPI_COMM_WORLD);
				}
				MPI_Send(vectorB, clm, MPI_FLOAT, i, 4, MPI_COMM_WORLD);
			}
			time_start = MPI_Wtime();
			for (i = 0; i < str && i < countStr; i++) {
				vectorC[i] = 0;
				for (j = 0; j < clm; j++)
					vectorC[i] += matrix[i][j] * vectorB[j];
			}
			time_end = MPI_Wtime();
			for (i = 1; i < size; i++) {
				float vectorCThreads[str];
				MPI_Recv(vectorCThreads, str, MPI_FLOAT, i, 5, MPI_COMM_WORLD, &status);
				for (j = i * countStr; j < str && j < (i + 1) * countStr; j++)
					vectorC[j] = vectorCThreads[j];
			}
		} else {
			countClm = clm / size + (clm % size != 0);
			float matrixCopy[clm][str];
			for (i = 0; i < clm; i++)
				for (j = 0; j < str; j++)
					matrixCopy[i][j] = matrix[j][i];
			int j = countClm;
			for (i = 1; i < size; i++) {
				MPI_Send(&str, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&clm, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
				MPI_Send(&countClm, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
				int end = j + countClm;
				for (; j < clm && j < end; j++)
					MPI_Send(matrixCopy[j], str, MPI_FLOAT, i, 3, MPI_COMM_WORLD);
				MPI_Send(vectorB + i * countClm, countClm, MPI_FLOAT, i, 4, MPI_COMM_WORLD);
			}
			time_start = MPI_Wtime();
			for (i = 0; i < str; i++) {
				vectorC[i] = 0;
				for (j = 0; j < countClm; j++)
					vectorC[i] += matrixCopy[j][i] * vectorB[j];
			}
			time_end = MPI_Wtime();
			for (i = 1; i < size; i++) {
				float vectorCThreads[str];
				MPI_Recv(vectorCThreads, str, MPI_FLOAT, i, 5, MPI_COMM_WORLD, &status);
				for (j = 0; j < str; j++)
					vectorC[j] += vectorCThreads[j];
			}
		}
		double max_time = time_end - time_start;
		printf("Time of %d-process = %lf\n", rank, time_end - time_start);
		for (int i = 0; i < size - 1; i++) { // прием и печать времени остальных процессов
			double time;
			MPI_Recv(&time, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			if (time > max_time)
				max_time = time;
		}
		printf("Max of time = %lf\n", max_time);
		writeVector(argv[3], str, vectorC);
		// вывод вектора С
		// for (i = 0; i < str; i++)
		// 	printf("%f ", vectorC[i]);
		// printf("\n");
		//
	}
	else
	{
		MPI_Recv(&str, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&clm, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		if (str >= clm) {
			MPI_Recv(&countStr, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
			float matrix[countStr][(int)clm];
			for (i = 0; i < countStr; i++)
				MPI_Recv(matrix[i], (int) clm, MPI_FLOAT, 0, 3, MPI_COMM_WORLD, &status);
			float vectorB[(int)clm];
			MPI_Recv(vectorB, (int)clm, MPI_FLOAT, 0, 4, MPI_COMM_WORLD, &status);
			float vectorC[(int)str];
			time_start = MPI_Wtime();
			for (i = rank * countStr; i < (int)str && i < (rank + 1) * countStr; i++) {
				vectorC[i] = 0;
				for (j = 0; j < (int) clm; j++)
					vectorC[i] += matrix[i - rank * countStr][j] * vectorB[j];
			}
			time_end = MPI_Wtime();
			MPI_Send(vectorC, (int)str, MPI_FLOAT, 0, 5, MPI_COMM_WORLD);
		} else {
			MPI_Recv(&countClm, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
			float matrix[countClm][(int)str];
			for (i = 0; i < countClm; i++)
				MPI_Recv(matrix[i], (int) str, MPI_FLOAT, 0, 3, MPI_COMM_WORLD, &status);
			float vectorB[(int)countClm];
			MPI_Recv(vectorB, countClm, MPI_FLOAT, 0, 4, MPI_COMM_WORLD, &status);
			float vectorC[(int)str];
			time_start = MPI_Wtime();
			for (i = 0; i < (int) str; i++) {
				vectorC[i] = 0;
				for (j = rank * countClm; j < (int) clm && j < (rank + 1) * countClm; j++)
					vectorC[i] += matrix[j - rank * countClm][i] * vectorB[j - rank * countClm];
			}
			time_end = MPI_Wtime();
			MPI_Send(vectorC, (int)str, MPI_FLOAT, 0, 5, MPI_COMM_WORLD);
		}
		double time = time_end - time_start;
		MPI_Send(&time, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		printf("Time of %d-process = %lf\n", rank, time);
	}
	MPI_Finalize();
	return 0;
}
