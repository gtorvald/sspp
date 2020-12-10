all:
	mpixlc -O2 -o go main.c

gen:
	gcc -Wall generating.c -o gen

clean:
	rm -f go gen
