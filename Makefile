# Nikolaos Giovanopoulos - CSD4613
# csd4613@csd.uoc.gr

all: cs345sh clean

cs345sh: utils.o main.o
	gcc -o cs345sh main.o utils.o

main.o:	utils.h main.c
	gcc -c main.c

utils.o: utils.h utils.c
	gcc -c utils.c

clean:
	rm -rf *.o

