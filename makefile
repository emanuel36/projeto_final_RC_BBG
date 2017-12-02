CC = gcc
INC = -I./inc

all: folder server client 

beaglebone: folder server client

server: server.o embedded_linux.o
	$(CC) obj/embedded_linux.o obj/server.o -o APP/server.bin

client: client.o
	$(CC) obj/client.o -o APP/client.bin

server.o:
	$(CC) -o server.o -c $(INC) src/server.c -o obj/server.o

client.o:
	$(CC) -o client.o -c $(INC) src/client.c -o obj/client.o 

embedded_linux.o: 
	$(CC) -o embedded_linux.o -c $(INC) src/embedded_linux.c -o obj/embedded_linux.o 

send:
	scp -r ..projeto_final_RC_BBG root@192.168.1.18:~/

folder: 
	mkdir -p obj APP

clean:
	rm -rf obj APP