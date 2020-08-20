CC=gcc

all: main

main: mm_seq.o mm_p.o
	$(CC) mm_seq.o -o mm_seq.out
	$(CC) mm_p.o -pthread -o mm_p.out
	@echo "use ./mm_seq.out -h and ./mm_p.out -h"

mm_seq.o: mm_seq.c
	$(CC) -c mm_seq.c

mm_p.o: mm_p.c
	$(CC) -c mm_p.c

clean:
	rm -f *.out *.o