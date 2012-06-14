
CFLAGS=-O3

natsort: main.o natsort.o

main.o: main.c natsort.h

natsort.o: natsort.c natsort.h
