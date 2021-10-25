all: server.c client.c
	gcc -Wall -o server.o server.c
	gcc -Wall -o client.o client.c

test: pthread_test.c
	gcc -Wall -o pthread_test.o pthread_test.c -pthread
clean:
	@rm -f *.o