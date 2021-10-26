all: server.c client.c
	gcc -Wall -pthread -o server.o server.c
	gcc -Wall -pthread -o client.o client.c

clean:
	@rm -f *.o