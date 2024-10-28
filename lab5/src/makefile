CC=gcc
CFLAGS=-I.
all : factorial.elf
factorial.elf : factorial.o
	$(CC) -o factorial.elf factorial.o -lpthread $(CFLAGS)
factorial.o : 
	$(CC) -o factorial.o -c factorial.c
clean :
	rm factorial.o factorial.elf