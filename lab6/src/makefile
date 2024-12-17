CC=gcc
CFLAGS=-I.
all : client.elf server.elf servers.txt
	./server.elf --port 0 --tnum 3 &
	./server.elf --port 1 --tnum 3 &
	./server.elf --port 2 --tnum 3 &
	./client.elf --k 20 --mod 59 --servers ryeerywu
client.elf : client.o
	$(CC) -o client.elf client.o -lpthread $(CFLAGS)
client.o : 
	$(CC) -o client.o -c client.c
server.elf : server.o
	$(CC) -o server.elf server.o -lpthread $(CFLAGS)
server.o : 
	$(CC) -o server.o -c server.c
clean :
	rm client.o client.elf server.o server.elf