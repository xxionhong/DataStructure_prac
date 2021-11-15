#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>
#include "singlylinkedlist.c"

#define MAX_CLIENT 10
#define BUFFER_SIZE 1024
#define PORT 12345

int cli_count = 0, flag = 0, server_socketFD;
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
Node *head = NULL;

void close_linkedlist()
{
    Node *temp = head;
    while (temp != NULL)
    {
        close(temp->data);
        temp = temp->next;
    }
    freeallNodes(head);
}

// using fcntl to set FD nonblocking
void setNonblocking(int fd)
{
    int opts;
    opts = fcntl(fd, F_GETFL);
    if (opts < 0)
    {
        perror("fcntl(sock,GETFL)");
        return;
    }

    opts = opts | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, opts) < 0)
    {
        perror("fcntl(sock,SETFL,opts)");
        return;
    }
}

void sig_handler(int a)
{
    flag = 1;
}

void *client_handler(void *fd)
{
    int sockfd = *(int *)fd;
    ssize_t n;
    char recv_buff[BUFFER_SIZE];
    memset(recv_buff, 0, BUFFER_SIZE);
    pthread_detach(pthread_self());
    while (!flag)
    {
        n = read(sockfd, recv_buff, BUFFER_SIZE);
        if (n == -1)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                continue;
            }
            perror("read\t");
            break;
        }
        else if (n == 0)
        {
            printf("sockfd %d left\n", sockfd);
            break;
        }
        else if (n > 0)
        {
            printf("%s < %d\n", recv_buff, sockfd);
            pthread_mutex_lock(&list_mutex);
            Node *temp = head;
            while (temp != NULL)
            {
                if (temp->data != sockfd)
                {
                    write(temp->data, recv_buff, strlen(recv_buff));
                }
                temp = temp->next;
            }
            pthread_mutex_unlock(&list_mutex);
            if (!strcmp(recv_buff, "exit"))
            {
                break;
            }
            memset(recv_buff, 0, BUFFER_SIZE);
        }
    }
    pthread_mutex_lock(&list_mutex);
    deleteNode(&head, sockfd);
    pthread_mutex_unlock(&list_mutex);
    close(sockfd);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    int connect_FD;
    struct sockaddr_in server_addr, cli_addr;
    pthread_t client_p;
    signal(SIGINT, sig_handler);

    if ((server_socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket\t");
        exit(EXIT_FAILURE);
    }
    setNonblocking(server_socketFD);
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socketFD, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind:\t");
        close(server_socketFD);
        exit(EXIT_FAILURE);
    }
    if (listen(server_socketFD, MAX_CLIENT) == -1)
    {
        perror("listen:\t");
        close(server_socketFD);
        exit(EXIT_FAILURE);
    }
    printf("Starting Server Side Function...\n");
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(server_socketFD, &rfds);
    while (!flag)
    {
        int nready = select(server_socketFD + 1, &rfds, NULL, NULL, NULL);
        if (nready <= 0)
        {
            continue;
        }
        if (FD_ISSET(server_socketFD, &rfds))
        {
            socklen_t cli_len = sizeof(cli_addr);
            connect_FD = accept(server_socketFD, (struct sockaddr *)&cli_addr, &cli_len);
            if (connect_FD == -1)
            {
                perror("accept\t");
                break;
            }
            else
            {
                printf("New client %d\n", connect_FD);
                if ((cli_count += 1) > MAX_CLIENT)
                {
                    printf("reach Max ...\n");
                    close(connect_FD);
                    continue;
                }
                setNonblocking(connect_FD);
                appendNode(&head, connect_FD);
                pthread_create(&client_p, NULL, client_handler, &connect_FD);
            }
        }
    }
    close_linkedlist();
    close(server_socketFD);
    return 0;
}
