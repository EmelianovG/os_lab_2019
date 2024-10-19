CC=gcc
CFLAGS=-I.
all : parallel.elf process_memory.elf parallel_sum.elf
parallel.elf : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel.elf utils.o find_min_max.o parallel_min_max.c $(CFLAGS)
parallel_sum.elf : parallel_sum.o utils.o sum.o
	$(CC) -o parallel_sum.elf parallel_sum.o utils.o sum.o -lpthread $(CFLAGS)
parallel_sum.o : 
	$(CC) -o parallel_sum.o -c parallel_sum.c
sum.o : sum.h
	$(CC) -o sum.o -c sum.c $(CFLAGS)
process_memory.elf : process_memory.o
	$(CC) -o process_memory.elf process_memory.o
process_memory.o : 
	$(CC) -o process_memory.o -c process_memory.c
utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)
clean :
	rm utils.o find_min_max.o parallel.elf process_memory.elf process_memory.o parallel_sum.elf parallel_sum.o