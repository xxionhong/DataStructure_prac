/*
    code from https://iter01.com/319359.html
    for study only
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#define PORT 8888
#define MAX_LINE 2048
#define LISTENQ 20

int main(int argc, char **argv)
{
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nready, client[FD_SETSIZE];

    ssize_t n, ret;
    fd_set rset, allset;

    char buf[MAX_LINE];
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;

    /*(1) 得到監聽描述符*/
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    /*(2) 繫結套接字*/
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    /*(3) 監聽*/
    listen(listenfd, LISTENQ);

    /*(4) 設定select*/
    maxfd = listenfd;
    maxi = -1;
    for (i = 0; i < FD_SETSIZE; ++i)
    {
        client[i] = -1;
    } // for
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    /*(5) 進入伺服器接收請求死迴圈*/
    while (1)
    {
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &rset))
        {
            /*接收客戶端的請求*/
            clilen = sizeof(cliaddr);

            printf("\naccpet connection~\n");

            if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0)
            {
                perror("accept error.\n");
                exit(1);
            } // if

            printf("accpet a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);

            /*將客戶連結套接字描述符新增到陣列*/
            for (i = 0; i < FD_SETSIZE; ++i)
            {
                if (client[i] < 0)
                {
                    client[i] = connfd;
                    break;
                } // if
            }     // for

            if (FD_SETSIZE == i)
            {
                perror("too many connection.\n");
                exit(1);
            } // if

            FD_SET(connfd, &allset);
            if (connfd > maxfd)
                maxfd = connfd;
            if (i > maxi)
                maxi = i;

            if (--nready < 0)
                continue;
        } // if

        for (i = 0; i <= maxi; ++i)
        {
            if ((sockfd = client[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &rset))
            {
                /*處理客戶請求*/
                printf("\nreading the socket~~~ \n");

                bzero(buf, MAX_LINE);
                if ((n = read(sockfd, buf, MAX_LINE)) <= 0)
                {
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                } // if
                else
                {
                    printf("clint[%d] send message: %s\n", i, buf);
                    if ((ret = write(sockfd, buf, n)) != n)

                    {
                        printf("error writing to the sockfd!\n");
                        break;
                    } // if
                }     // else
                if (--nready <= 0)
                    break;
            } // if
        }     // for
    }         // while
}