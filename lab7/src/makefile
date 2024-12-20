CC=gcc
FLAGS=-I.

all: tcpclient.elf tcpserver.elf udpclient.elf udpserver.elf

tcpclient.elf:
	$(CC) tcpclient.c -o tcpclient.elf $(FLAGS)

tcpserver.elf:
	$(CC) tcpserver.c -o tcpserver.elf $(FLAGS)

udpclient.elf:
	$(CC) udpclient.c -o udpclient.elf $(FLAGS)

udpserver.elf:
	$(CC) udpserver.c -o udpserver.elf $(FLAGS)

clean:
	rm tcpclient.elf tcpserver.elf udpclient.elf udpserver.elf