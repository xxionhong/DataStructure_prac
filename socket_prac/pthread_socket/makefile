CC = gcc
CFLAGS = -Wall -pthread

all: server.c client.c
	${CC} ${CFLAGS} -o server.o server.c
	${CC} ${CFLAGS} -o client.o client.c

clean:
	@rm -f *.o