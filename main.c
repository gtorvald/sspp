#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <papi.h>
#include <time.h>

#define min(a,b) {a < b ? a : b}

int     handleError(int code) {
        printf ("Error: %d\n", code);
        return 1;
}

float	**readMatrix(char *file, size_t *size) {
	float	**matrix;
	char	type;
	FILE	*f;
	f = fopen(file, "rb");
	fread(&type, sizeof(char), 1, f);

	fread(size, sizeof(size_t), 1, f);
	fread(size, sizeof(size_t), 1, f);
	matrix = (float**) malloc(sizeof(float*) * *size);
        for (int i = 0; i < *size; i++)
            matrix[i] = malloc(sizeof(float) * *size);
	for (int i = 0; i < *size; i++)
		for (int j = 0; j < *size; j++)
			fread(&(matrix[i][j]), sizeof(float), 1, f);
	fclose(f);
	return matrix;
}

void	writeMatrix(char *file, size_t size, float **matrixC) {
	FILE *f;
	f = fopen(file, "wb");
	char typeF = 'f';

	fwrite(&typeF, sizeof(char), 1, f);
	fwrite(&size, sizeof(size_t), 1, f);
	fwrite(&size, sizeof(size_t), 1, f);
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			fwrite(&(matrixC[i][j]), sizeof(float), 1, f);
	fclose(f);
}

float   **algorithm(float **matrixA, float **matrixB, size_t size, char *type, int size_block) {
        float   **matrixC = (float**) malloc(sizeof(float*) * size);

        for (int i = 0; i < size; i++)
           	matrixC[i] = malloc(sizeof(float) * size);
        if (!strcmp(type, "ikj"))
	        for (int i = 0; i < size; i += size_block)
	                for (int k = 0; k < size; k += size_block)
	                        for (int j = 0; j < size; j += size_block) {
	                        	int ii_max = min(size, i + size_block);
	                        	int jj_max = min(size, j + size_block);
	                        	int kk_max = min(size, k + size_block);
								for (int ii = i; ii < ii_max; ii++)
									for (int kk = k; kk < kk_max; kk++)
										for (int jj = j; jj < jj_max; jj++)
											matrixC[ii][jj] += matrixA[ii][kk] * matrixB[kk][jj];
	                        }
	  	else
	  		for (int i = 0; i < size; i += size_block)
	                for (int j = 0; j < size; j += size_block)
	                        for (int k = 0; k < size; k += size_block) {
	                        	int ii_max = min(size, i + size_block);
	                        	int jj_max = min(size, j + size_block);
	                        	int kk_max = min(size, k + size_block);
								for (int ii = i; ii < ii_max; ii++)
									for (int jj = j; jj < jj_max; jj++)
										for (int kk = k; kk < kk_max; kk++)
											matrixC[ii][jj] += matrixA[ii][kk] * matrixB[kk][jj];
	                        }
        return matrixC;
}

int     main(int argc, char **argv) { 
        size_t		size;
        float     	**matrixA, **matrixB, **matrixC;
        int     	eventSet = PAPI_NULL;
        int   		eventCodes[4] = {PAPI_L1_DCM, PAPI_L1_ICM, PAPI_FP_OPS, PAPI_TOT_CYC};
        long long  	values[4];

        matrixA = readMatrix(argv[1], &size);
        matrixB = readMatrix(argv[2], &size);
        if(PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT )
                return handleError(0);
        if (PAPI_create_eventset(&eventSet) != PAPI_OK)
                return handleError(1);
        if (PAPI_add_events(eventSet, eventCodes, 4) != PAPI_OK)
                return handleError(2);
        if (PAPI_start(eventSet) != PAPI_OK)
                return handleError(3);
        matrixC = algorithm(matrixA, matrixB, size, argv[4], atoi(argv[5]));
        if (PAPI_stop(eventSet, values) != PAPI_OK)
                return handleError(4);
        if (PAPI_remove_events(eventSet,eventCodes, 4) != PAPI_OK)
                return handleError(5);
        if (PAPI_destroy_eventset(&eventSet) != PAPI_OK)
                return handleError(6);
        PAPI_shutdown();
        writeMatrix(argv[3], size, matrixC);
        if (argc == 7) {
		    printf("%f\n", (float) values[3] / CLOCKS_PER_SEC); // time
			printf("%lld\n", values[0] + values[1]); // L1
			printf("%ld\n", values[3]); // CPU
			printf("%lld\n", values[2]); // FLP
        }
        return 0;
}
