#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct info
{
	int		sqrt_max;
	int		max;
	int		min;
	int		size;
	int		rank;
	int		str[50000000];
	int		count;
	int		data[100000000];
	int		step; 	
	clock_t	time;
} Information;

int		maximum(int a, int b) {
	return a > b ? a : b;
}

void	*doWork(void *param) {
	Information *info = (Information*)param;
	int str[info->sqrt_max];
	int i, j;
	// инициализация массива
	clock_t time_start = clock();
	for (i = 0; i < info->sqrt_max; i++) {
		str[i] = i + 1;
		if (str[i] == 1)
			str[i] = 0;
	}
	// поиск первых простых чисел до sqrt(max)
	for (i = 1; i < info->sqrt_max; i++)
		if (str[i])
			for (j = i + str[i]; j < info->sqrt_max; j+= str[i])
				str[j] = 0;
	// вычеркивание чисел во всех процессах
	int step = (info->max - info->min) / info->size + ((info->max - info->min) % info->size != 0);
	int begin = info->min + info->rank * step + 1; // число , с которого нужно начинать подсчет
	int data[step];
	for (i = 0; i < step; i++) {
		if (i + begin - 1 >= info->max)
			data[i] = 0;
		else
			data[i] = i + begin;
	}
	for (i = 0; i < info->sqrt_max; i++) {
		if (str[i]) {
			int j = (begin / str[i] + (begin % str[i] != 0)) * str[i] - 1;
			for (; j < begin + step && j < info->max; j += str[i]) {
				data[j - begin + 1] = 0;
			}
		}
	}
	info->time = (float) (clock() - time_start) / CLOCKS_PER_SEC;
	for (i = 0; i < step; i++)
		info->data[i] = data[i];
	for (i = 0; i < info->sqrt_max; i++)
		info->str[i] = str[i];
	info->step = step;
	return NULL;
}

int		main(int argc, char **argv)
{
	int			size = atoi(argv[4]), i, j;
	pthread_t	pthreads[size];
	Information	info[size];
	for (i = 0; i < size; i++) {
		info[i].max = atoi(argv[2]);
		info[i].sqrt_max = info[i].max / 2;
		info[i].min = maximum(info[i].sqrt_max, atoi(argv[1]) - 1);
		info[i].size = size;
	}
	for (i = 0; i < size; i++) {
		info[i].rank = i;
		if (pthread_create(&pthreads[i], NULL, doWork, &info[i])) {
		    fprintf(stderr, "Error creating a new thread\n");
			return 1;
		}
	}
		for (i = 0; i < size; i++) {
		if (pthread_join(pthreads[i], NULL)) {
			fprintf(stderr, "Error joining the thread\n");
			return 2;
		}
	}
	double sumTime = 0, maxTime = 0;
	int count = 0;
	FILE *f = fopen(argv[3], "w");
	for (i = 0; i < size; i++) {
		sumTime += info[i].time;
		if (info[i].time > maxTime)
			maxTime = info[i].time;
		int begin = atoi(argv[1]);
		if (i == 0)
			for (j = 0; j < info[i].sqrt_max; j++) {
				if (info[i].str[j] >= begin) {
					count++;
					fprintf(f, "%d ", info[i].str[j]);
				}
			}
		for (j = 0; j < info[i].step; j++)
			if (info[i].data[j] >= begin) {
				count++;
				fprintf(f, "%d ", info[i].data[j]);
			}
	}
	fprintf(f, "\n");
	printf("Summ of time = %lf\n", sumTime);
	printf("Max of time = %lf\n", maxTime);
	printf("Count of numbers = %d\n", count);
	return 0;
}
