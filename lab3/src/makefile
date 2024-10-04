CC=gcc
CFLAGS=-I.
all : sequential.elf parallel.elf seq_exec.elf
sequential.elf : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o sequential.elf find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel.elf : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel.elf utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

seq_exec.elf : seq_exec.o
	$(CC) -o seq_exec.elf seq_exec.o $(CFLAGS)

seq_exec.o :
	$(CC) -o seq_exec.o -c seq_exec.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o seq_exec.o sequential.elf parallel.elf seq_exec.elf