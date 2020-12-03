#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int maximum(int a, int b) {
	return a > b ? a : b;
}

int main(int argc, char **argv)
{
	int size, rank, i, max = atoi(argv[2]), sqrt_max = sqrt(max);
	int *str = malloc(sqrt_max * sizeof(int));
	// int str[sqrt_max];
	int min = maximum(sqrt_max, atoi(argv[1]) - 1);

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;
	MPI_Barrier(MPI_COMM_WORLD);
	// инициализация массива
	double time_start = MPI_Wtime();
	for (int i = 0; i < sqrt_max; i++) {
		str[i] = i + 1;
		if (str[i] == 1)
			str[i] = 0;
	}
	// поиск первых простых чисел до sqrt(max)
	for (int i = 1; i < sqrt_max; i++)
		if (str[i])
			for (int j = i + str[i]; j < sqrt_max; j+= str[i])
				str[j] = 0;
	// вычеркивание чисел во всех процессах
	int step = (max - min) / size + ((max - min) % size != 0);
	int begin = min + rank * step + 1; // число , с которого нужно начинать подсчет
	int *data = malloc((step + 1) * sizeof(int));
	// int data[step];
	for (int i = 0; i < step; i++) {
		if (i + begin - 1 >= max)
			data[i] = 0;
		else
			data[i] = i + begin;
	}
	for (int i = 0; i < sqrt_max; i++) {
		if (str[i]) {
			int j = (begin / str[i] + (begin % str[i] != 0)) * str[i] - 1;
			for (; j < begin + step && j < max; j += str[i]) {
				data[j - begin + 1] = 0;
			}
		}
	}
	double time_end = MPI_Wtime();
	// отправка простых чисел и времени 0-ому процессу
	if (rank) {
		MPI_Send(data, step, MPI_INT, 0, 2, MPI_COMM_WORLD);
		double time = time_end - time_start;
		MPI_Send(&time, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		// printf("Time of %d-process = %lf\n", rank, time);
	} else { // прием простых чисел и времени 0-вым процессом от остальных
		FILE *f;
		int count = 0;
		// f = fopen(argv[3], "w");
		for (int i = atoi(argv[1]) - 1; i < sqrt_max; i++) // печать простых чисел до sqrt(n)
			if (str[i]) {
				count++;
				// fprintf(f, "%d ", str[i]);
			}
		for (int i = 0; i < step; i++) // печать своих простых чисел
			if (data[i]) {
				count++;
				// fprintf(f, "%d ", data[i]);
			}
		for (int i = 0; i < size - 1; i++) { // прием и печать простых чисел остальных процессов
			MPI_Recv(data, step, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
			for (int i = 0; i < step; i++)
				if (data[i]) {
					count++;
					// fprintf(f, "%d ", data[i]);
				}
		}
		// fprintf(f, "\n");
		// fclose(f);
		double sumTime = time_end - time_start, max_time = sumTime;
		// printf("Time of %d-process = %lf\n", rank, time_end - time_start);
		for (int i = 0; i < size - 1; i++) { // прием и печать времени остальных процессов
			double time;
			MPI_Recv(&time, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			if (time > max_time)
				max_time = time;
			sumTime += time;
		}
		// printf("Summ of time = %lf\n", sumTime);
		// printf("Max of time = %lf\n", max_time);
		printf("%d\n", count);
	}
	free(str);
	free(data);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
