#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
	int size, rank, i, min = atoi(argv[1]), max = atoi(argv[2]);
	int str[max];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;
	MPI_Barrier(MPI_COMM_WORLD);
	// инициализация массива
	double time_start = MPI_Wtime();
	for (int i = 0; i < max; i++) {
		str[i] = i + 1;
		if (str[i] == 1)
			str[i] = 0;
	}
	// поиск первых простых чисел до sqrt(max)
	for (int i = 0; i < sqrt(max); i++) {
		int flag = str[i];
		for (int j = 2; j <= sqrt(str[i]); j++)
			if (str[i] % j == 0) {
				flag = 0;
				break;
			}
		str[i] = flag;
	}
	// вычеркивание числе во всех процессах
	int step = roundf((float)(max - sqrt(max)) / size + 1);
	int begin = sqrt(max) + rank * step;
	int data[step];
	for (int i = 0; i < step; i++)
		data[i] = 0;
	for (int i = begin; i < begin + step && i < max; i++) {
		for (int j = 0; j < sqrt(max); j++)
			if (str[j] && (str[i] % str[j] == 0)) {
				str[i] = 0;
				break;
			}
		data[i - begin] = str[i];
	}
	double time_end = MPI_Wtime();
	// отправка простых чисел и времени 0-ому процессу
	if (rank) {
		MPI_Send(data, step, MPI_INT, 0, 2, MPI_COMM_WORLD);
		double time = time_end - time_start;
		MPI_Send(&time, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		printf("Time of %d-process = %lf\n", rank, time);
	} else { // прием простых чисел и времени 0-вым процессом от остальных
		FILE *f;
		int count = 0;
		f = fopen(argv[3], "w");
		for (int i = min - 1; i < sqrt(max); i++) // печать простых чисел до sqrt(n)
			if (str[i]) {
				count++;
				fprintf(f, "%d ", str[i]);
			}
		for (int i = 0; i < step; i++) // печать своих простых чисел
			if (data[i]) {
				count++;
				fprintf(f, "%d ", data[i]);
			}
		for (int i = 0; i < size - 1; i++) { // прием и печать простых чисел остальных процессов
			MPI_Recv(data, step, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
			for (int i = 0; i < step; i++)
				if (data[i]) {
					count++;
					fprintf(f, "%d ", data[i]);
				}
		}
		fprintf(f, "\n");
		fclose(f);
		double sumTime = time_end - time_start;
		printf("Time of %d-process = %lf\n", rank, time_end - time_start);
		for (int i = 0; i < size - 1; i++) { // прием и печать времени остальных процессов
			double time;
			MPI_Recv(&time, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			sumTime += time;
		}
		printf("Summ of time = %lf\n", sumTime);
		printf("Count of numbers = %d\n", count);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
