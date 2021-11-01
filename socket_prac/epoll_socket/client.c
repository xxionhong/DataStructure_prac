#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024
#define PORT 12345
int left_flag = 0;

void send_and_recv(int connfd)
{
    FILE *fp = stdin;
    char send_buff[BUFFER_SIZE];
    char recv_buff[BUFFER_SIZE];
    fd_set rset;
    FD_ZERO(&rset);
    int maxfd = (fileno(fp) > connfd ? fileno(fp) : connfd + 1);
    int n;

    memset(send_buff, 0, BUFFER_SIZE);
    memset(recv_buff, 0, BUFFER_SIZE);
    while (!left_flag)
    {
        // add to select Fdset
        FD_SET(fileno(fp), &rset);
        FD_SET(connfd, &rset);

        // select
        if (select(maxfd, &rset, NULL, NULL, NULL) == -1)
        {
            perror("select\t");
            left_flag = 1;
            exit(EXIT_FAILURE);
        }

        // if connfd from server change, means new data in
        if (FD_ISSET(connfd, &rset))
        {
            n = read(connfd, recv_buff, BUFFER_SIZE);
            if (n == 0)
            {
                left_flag = 1;
                break;
            }
            else if (n == -1)
            {
                perror("read\t");
                left_flag = 1;
                break;
            }
            else
            {
                recv_buff[strlen(recv_buff)] = '\0';
                printf("%s\n", recv_buff);
            }
            memset(recv_buff, 0, BUFFER_SIZE);
        }
        // if terminal command in
        if (FD_ISSET(fileno(fp), &rset))
        {
            if (fgets(send_buff, BUFFER_SIZE, fp) == NULL)
            {
                left_flag = 1;
                break;
            }
            else
            {
                send_buff[strlen(send_buff) - 1] = '\0';
                write(connfd, send_buff, strlen(send_buff));
                if (strcmp(send_buff, "exit") == 0)
                {
                    left_flag = 1;
                    return;
                }
            }
            memset(send_buff, 0, BUFFER_SIZE);
        }
    }
}

int main(int argc, char **argv)
{
    int connfd;
    struct sockaddr_in servaddr;

    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket\t");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    // connect to server
    if (connect(connfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect\t");
        exit(EXIT_FAILURE);
    }
    printf("connect successfully!\n");
    send_and_recv(connfd);
    close(connfd);
    return 0;
}
