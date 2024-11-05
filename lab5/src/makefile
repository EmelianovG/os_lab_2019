CC=gcc
CFLAGS=-I.
all : factorial.elf deadlock.elf
factorial.elf : factorial.o
	$(CC) -o factorial.elf factorial.o -lpthread $(CFLAGS)
factorial.o : 
	$(CC) -o factorial.o -c factorial.c
deadlock.elf : deadlock.o
	$(CC) -o deadlock.elf deadlock.o -lpthread $(CFLAGS)
deadlock.o : 
	$(CC) -o deadlock.o -c deadlock.c
clean :
	rm factorial.o factorial.elf deadlock.o deadlock.elf