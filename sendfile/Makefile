CC = gcc
CFLAGS = -Wall

client: tcp_client.c
	${CC} ${CFLAGS} tcp_client.c -o client

server: tcp_server.c
	${CC} ${CFLAGS} tcp_server.c -o server

clean:
	rm -f *.o *~
