#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define PORT 12345
int server_left = 0;

void sig_handler(int sig_num)
{
    server_left = 1;
}

int main(int argc, char **argv)
{
    int listenfd, connfd, sockfd, maxfd, maxi, i;
    int nready, client[FD_SETSIZE];
    fd_set rset, allset;
    char recv_buff[BUFFER_SIZE];
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;
    signal(SIGINT, sig_handler);
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("listen\t");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("bind\t");
        exit(EXIT_FAILURE);
    }

    if (listen(listenfd, 100) == -1)
    {
        perror("listen\t");
        exit(EXIT_FAILURE);
    }

    maxfd = listenfd;
    maxi = -1;

    for (i = 0; i < FD_SETSIZE; i++)
    {
        client[i] = -1;
    }

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while (!server_left)
    {
        rset = allset;
        if ((nready = select(maxfd + 1, &rset, NULL, NULL, NULL)) == -1)
        {
            perror("select\t");
            server_left = 1;
            break;
        }

        if (nready <= 0)
        {
            continue;
        }

        if (FD_ISSET(listenfd, &rset))
        {
            clilen = sizeof(cliaddr);

            if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) == -1)
            {
                perror("accpet\t");
                continue;
            }
            printf("New client %d connected \n", connfd);
            for (i = 0; i < FD_SETSIZE; i++)
            {
                if (client[i] < 0)
                {
                    client[i] = connfd;
                    break;
                }
            }

            if (i == FD_SETSIZE)
            {
                printf("Reach Max... ");
                close(connfd);
                continue;
            }
            FD_SET(connfd, &allset);
            if (connfd > maxfd)
            {
                maxfd = connfd;
            }
            if (i > maxi)
            {
                maxi = i;
            }
        }

        for (i = 0; i <= maxi; i++)
        {
            if ((sockfd = client[i]) > 0)
            {
                if (FD_ISSET(sockfd, &rset))
                {
                    memset(recv_buff, 0, BUFFER_SIZE);
                    ssize_t n = read(sockfd, recv_buff, BUFFER_SIZE);
                    if (n < 0)
                    {
                        perror("read\t");
                        close(sockfd);
                        FD_CLR(sockfd, &allset);
                        client[i] = -1;
                        continue;
                    }
                    if (n == 0)
                    {
                        printf("SockFD %d left!\n", sockfd);
                        close(sockfd);
                        FD_CLR(sockfd, &allset);
                        client[i] = -1;
                        continue;
                    }

                    printf("%s < %d\n", recv_buff, sockfd);
                    for (int j = 0; j < FD_SETSIZE; j++)
                    {
                        if (client[j] > 0 && i != j)
                        {
                            write(client[j], recv_buff, strlen(recv_buff));
                        }
                    }

                    if (strcmp(recv_buff, "exit") == 0)
                    {
                        close(sockfd);
                        FD_CLR(sockfd, &allset);
                        client[i] = -1;
                        continue;
                    }
                }
            }
        }
    }
    for (int i = 0; i < FD_SETSIZE; i++)
    {
        if (client[i] > 0)
        {
            close(client[i]);
        }
    }
    close(listenfd);
    return 0;
}
