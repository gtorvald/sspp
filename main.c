#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void	writeSizes(char *file, int N) {
	FILE *f;
	f = fopen(file, "wb");
	char typeF = 'f';
	int i, j, N_local = N;

	fwrite(&typeF, sizeof(char), 1, f);
	fwrite(&N_local, sizeof(size_t), 1, f);
	fwrite(&N_local, sizeof(size_t), 1, f);
	fclose(f);
}

int		readSizes(char *file) {
	char	type;
	int		N;
	FILE	*f;
	f = fopen(file, "rb");
	fread(&type, sizeof(char), 1, f);
	fread(&N, sizeof(size_t), 1, f);
	fclose(f);
	return N;
}

int		main(int argc, char **argv) {
	int size, rank, local_array_size;
	int gsizes[2], lsizes[2], dims[3], periods[3], coords[3], start_indices[2];
	int N, K;
	MPI_Comm comm;
	MPI_Datatype filetypeA, filetypeB, filetypeC;
	MPI_File f;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	K = cbrt(size);
	N = readSizes(argv[1]);
	gsizes[0] = N;
	gsizes[1] = N;
	lsizes[0] = N / K;
	lsizes[1] = N / K;
	dims[0] = K;
	dims[1] = K;
	dims[2] = K;
	periods[0] = 1;
	periods[1] = 1;
	periods[2] = 1;
	MPI_Cart_create(MPI_COMM_WORLD, 3, dims, periods, 0, &comm);
	MPI_Comm_rank(comm, &rank);
	MPI_Cart_coords(comm, rank, 3, coords);
	start_indices[0] = coords[1] * lsizes[0];
	start_indices[1] = coords[2] * lsizes[1];
	MPI_Type_create_subarray(2, gsizes, lsizes, start_indices, MPI_ORDER_C, MPI_DOUBLE, &filetypeA);
	MPI_Type_commit(&filetypeA);
	MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &f);
	MPI_File_set_view(f, sizeof(char) + 2 * sizeof(size_t), MPI_DOUBLE, filetypeA, "native", MPI_INFO_NULL);
	local_array_size = lsizes[0] * lsizes[1];
	double *arrayA = malloc(sizeof(double) * local_array_size);
	MPI_File_read_all(f, arrayA, local_array_size, MPI_DOUBLE, &status);
	// for (int i = 0; i < local_array_size; i++) {
	// 	printf("%lf ", arrayA[i]);
	// 	if (i % (N / K) == 0)
	// 		printf("\n");
	// }
	MPI_File_close(&f);

	start_indices[0] = coords[0] * lsizes[0];
	start_indices[1] = coords[2] * lsizes[1];
	MPI_Type_create_subarray(2, gsizes, lsizes, start_indices, MPI_ORDER_C, MPI_DOUBLE, &filetypeB);
	MPI_Type_commit(&filetypeB);
	MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_RDONLY, MPI_INFO_NULL, &f);
	MPI_File_set_view(f, sizeof(char) + 2 * sizeof(size_t), MPI_DOUBLE, filetypeB, "native", MPI_INFO_NULL);
	double *arrayB = malloc(sizeof(double) * local_array_size);
	MPI_File_read_all(f, arrayB, local_array_size, MPI_DOUBLE, &status);
	// for (int i = 0; i < local_array_size; i++) {
	// 	printf("%lf ", arrayB[i]);
	// 	if (i % (N / K) == 0)
	// 		printf("\n");
	// }
	MPI_File_close(&f);
	double *arrayC = malloc(sizeof(double) * local_array_size);
	for (int i = 0; i < local_array_size; i++)
		arrayC[i] = 0;
	for (int i = 0; i < lsizes[0]; i++)
		for (int j = 0; j < lsizes[0]; j++)
			for (int k = 0; k < lsizes[0]; k++)
				arrayC[i * lsizes[0] + j] += arrayA[i * lsizes[0] + k] * arrayB[k * lsizes[0] + j];
	if (coords[2] != 0) {
		printf("coords = %d %d %d; rank = %d; send - %d\n", coords[0], coords[1], coords[2], rank, rank / K * K);
		MPI_Send(arrayC, local_array_size, MPI_DOUBLE, rank / K * K, 1, MPI_COMM_WORLD);
	} else {
		double *arrayC_recv = malloc(sizeof(double) * local_array_size);
		for (int i = 1; i < K; i++) {
			printf("coords = %d %d %d; rank = %d; recv - %d\n", coords[0], coords[1], coords[2], rank, rank + i);
			MPI_Recv(arrayC_recv, local_array_size, MPI_DOUBLE, rank + i, 1, MPI_COMM_WORLD, &status);
			for (int j = 0; j < local_array_size; j++)
				arrayC[j] += arrayC_recv[j];
		}
		free(arrayC_recv);
		for (int i = 0; i < local_array_size; i++) {
			printf("%lf ", arrayC[i]);
			if (i % (N / K) == 0)
				printf("\n");
		}
		if (rank == 0) {
			writeSizes(argv[3], N);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		start_indices[0] = coords[0] * lsizes[0];
		start_indices[1] = coords[1] * lsizes[1];
		MPI_Type_create_subarray(2, gsizes, lsizes, start_indices, MPI_ORDER_C, MPI_DOUBLE, &filetypeC);
		MPI_Type_commit(&filetypeC);
		MPI_File_open(MPI_COMM_WORLD, argv[3], MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &f);
		MPI_File_set_view(f, sizeof(char) + 2 * sizeof(size_t), MPI_DOUBLE, filetypeC, "native", MPI_INFO_NULL);
		MPI_File_write_all(f, arrayC, local_array_size, MPI_DOUBLE, &status);
		MPI_File_close(&f);
	}
	free(arrayA);
	free(arrayB);
	free(arrayC);
	MPI_Finalize();
	return 0;
}