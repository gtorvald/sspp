polus:
	module load SpectrumMPI/10.1.0
	mpixlc -o go main.c

mpi:
	mpixlc -o go main.c

ptr:
	gcc -lpthread -o go main_pthreads.c