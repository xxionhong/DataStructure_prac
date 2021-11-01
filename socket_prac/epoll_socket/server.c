#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include "singlylinkedlist.c"

#define CONNECT_SIZE 256

#define PORT 12345
#define BUFFER_SIZE 2048

// using fcntl to set FD nonblocking
void setNonblocking(int sockfd)
{
    int opts;
    opts = fcntl(sockfd, F_GETFL);
    if (opts < 0)
    {
        perror("fcntl(sock,GETFL)");
        return;
    }

    opts = opts | O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, opts) < 0)
    {
        perror("fcntl(sock,SETFL,opts)");
        return;
    }
}

int main(int argc, char **argv)
{
    int i, listenfd, connfd, sockfd, nfds;
    ssize_t n, ret;
    char recv_buff[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    Node *head = NULL;
    struct epoll_event ev, events[20];

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("listen\t");
        exit(EXIT_FAILURE);
    }
    setNonblocking(listenfd);

    int epfd = epoll_create(CONNECT_SIZE);
    ev.data.fd = listenfd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

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
    while (1)
    {
        nfds = epoll_wait(epfd, events, CONNECT_SIZE, -1);
        if (nfds <= 0)
            continue;
        for (i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == listenfd)
            {
                socklen_t clilen = sizeof(cliaddr);
                if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0)
                {
                    perror("accept\t");
                    close(listenfd);
                    exit(EXIT_FAILURE);
                }
                appendNode(&head, connfd);
                printf("accpet a new client: %d\n", connfd);
                setNonblocking(connfd);
                ev.data.fd = connfd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
            }
            // read from client
            else if (events[i].events & EPOLLIN)
            {
                if ((sockfd = events[i].data.fd) < 0)
                    continue;
                memset(recv_buff, 0, BUFFER_SIZE);
                if ((n = read(sockfd, recv_buff, BUFFER_SIZE)) <= 0)
                {
                    close(sockfd);
                    events[i].data.fd = -1;
                }
                else
                {
                    recv_buff[n] = '\0';
                    printf("%s < %d\n", recv_buff, sockfd);
                    if (!strcmp(recv_buff, "exit"))
                    {
                        deleteNode(&head, sockfd);
                        close(sockfd);
                        continue;
                    }
                    ev.data.fd = sockfd;
                    ev.events = EPOLLOUT | EPOLLET;
                    epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
                }
            }
            else if (events[i].events & EPOLLOUT)
            {
                if ((sockfd = events[i].data.fd) < 0)
                    continue;
                Node *temp = head;
                while (temp != NULL)
                {
                    if (temp->data != sockfd)
                    {
                        if ((ret = write(temp->data, recv_buff, n)) != n)
                        {
                            perror("write\t");
                            break;
                        }
                    }
                    temp = temp->next;
                }
                ev.data.fd = sockfd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
            }
        }
    }
    free(events);
    close(epfd);
    exit(0);
}