all:
	mpixlc -o go main.c

gen:
	gcc -Wall generating.c -o gen

clean:
	rm -f go gen