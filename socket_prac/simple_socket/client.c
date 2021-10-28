#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])

{

    int sockfd;
    char buf[2000];
    char message[] = {"Message from client!\n"};
    struct sockaddr_in address;

    // TCP socket

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    // Initial, connect to port 2323

    address.sin_family = AF_INET;
    address.sin_port = htons(12345);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(address.sin_zero), 8);

    // Connect to server

    if (connect(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        exit(1);
    }

    printf("Enter a message: \n>");
    while (1)
    {

        scanf("%s", buf);
        send(sockfd, buf, strlen(buf), 0);
        if (!strcmp(buf, "exit"))
        {
            close(sockfd);
            exit(EXIT_SUCCESS);
        }
        printf(">");
    }

    return 0;
}