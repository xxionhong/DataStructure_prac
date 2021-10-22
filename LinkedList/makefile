CC = gcc
CFLAGS = -Wall -O2 -Wno-unused-function

main: *.o
	${CC} ${CFLAGS} *.o -o main

*.o: *.c
	${CC} ${CFLAGS} *.c -c

clean:
	@rm -rf *.o main