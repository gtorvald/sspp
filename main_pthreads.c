#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define THREADS 64
#define SIZE_STR 10000 // sqrt(10^8)
#define SIZE_DATA (100000000 - SIZE_STR) / THREADS + 1

typedef struct info
{
	int		sqrt_max;
	int		max;
	int		min;
	int		size;
	int		rank;
	int		str[SIZE_STR];
	int		count;
	int		data[SIZE_DATA];
	int		step; 	
	double	time;
} Information;

int		maximum(int a, int b) {
	return a > b ? a : b;
}

void	*doWork(void *param) {
	Information *info = (Information*)param;
	int i, j;
	struct timespec time_start, time_finish;
	// инициализация массива
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time_start);
	for (i = 0; i < info->sqrt_max; i++) {
		info->str[i] = i + 1;
		if (info->str[i] == 1)
			info->str[i] = 0;
	}
	// поиск первых простых чисел до sqrt(max)
	for (i = 1; i < info->sqrt_max; i++)
		if (info->str[i])
			for (j = i + info->str[i]; j < info->sqrt_max; j+= info->str[i])
				info->str[j] = 0;
	// вычеркивание чисел во всех процессах
	int step = (info->max - info->min) / info->size + ((info->max - info->min) % info->size != 0);
	// printf("%d\n", step);
	// printf("%d\n", SIZE_DATA);
	int begin = info->min + info->rank * step + 1; // число , с которого нужно начинать подсчет
	for (i = 0; i < step; i++) {
		if (i + begin - 1 >= info->max)
			info->data[i] = 0;
		else
			info->data[i] = i + begin;
	}
	for (i = 0; i < info->sqrt_max; i++) {
		if (info->str[i]) {
			int j = (begin / info->str[i] + (begin % info->str[i] != 0)) * info->str[i] - 1;
			for (; j < begin + step && j < info->max; j += info->str[i]) {
				info->data[j - begin + 1] = 0;
			}
		}
	}
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time_finish);
	info->time = (double) (time_finish.tv_sec - time_start.tv_sec + 1e-9 * (time_finish.tv_nsec - time_start.tv_nsec));
	info->step = step;
	return NULL;
}

int		main(int argc, char **argv)
{
	int			size = atoi(argv[3]), i, j;
	pthread_t	pthreads[size];
	Information	info[size];
	for (i = 0; i < size; i++) {
		info[i].max = atoi(argv[2]);
		info[i].sqrt_max = SIZE_STR;
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
	// FILE *f = fopen(argv[3], "w");
	for (i = 0; i < size; i++) {
		sumTime += info[i].time;
		if (info[i].time > maxTime)
			maxTime = info[i].time;
		int begin = atoi(argv[1]);
		if (i == 0)
			for (j = 0; j < info[i].sqrt_max; j++) {
				if (info[i].str[j] >= begin) {
					count++;
					// fprintf(f, "%d ", info[i].str[j]);
				}
			}
		for (j = 0; j < info[i].step; j++)
			if (info[i].data[j] >= begin) {
				count++;
				// fprintf(f, "%d ", info[i].data[j]);
			}
	}
	// fprintf(f, "\n");
	// printf("%lf\n", sumTime);
	// printf("%lf\n", maxTime);
	printf("%d\n", count);
	return 0;
}
