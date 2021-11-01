#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "singlylinkedlist.c"

#define MAX_CLIENT 10
#define BUFFER_SIZE 1024
#define PORT 12345

int cli_count = 0, server_flag = 0;
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
Node *head = NULL;

void sig_handler(int a)
{
    server_flag = 1;
    exit(EXIT_FAILURE);
}

void *client_handler(void *fd)
{
    int sockfd = *(int *)fd;
    char recv_buff[BUFFER_SIZE];
    memset(recv_buff, 0, BUFFER_SIZE);
    int left_flag = 0;
    while (!left_flag)
    {
        ssize_t n = read(sockfd, recv_buff, BUFFER_SIZE);
        if (n < 0)
        {
            perror("read\t");
            left_flag = 1;
            break;
        }
        if (n == 0)
        {
            printf("sockfd %d left\n", sockfd);
            left_flag = 1;
            break;
        }
        printf("%s < %d\n", recv_buff, sockfd);
        pthread_mutex_lock(&list_mutex);
        while (head != NULL)
        {
            if (head->data != sockfd)
            {
                write(head->data, recv_buff, strlen(recv_buff));
            }
            head = head->next;
        }
        pthread_mutex_unlock(&list_mutex);
        if (!strcmp(recv_buff, "exit"))
        {
            break;
        }
        memset(recv_buff, 0, BUFFER_SIZE);
    }

    pthread_mutex_lock(&list_mutex);
    deleteNode(&head, sockfd);
    pthread_mutex_unlock(&list_mutex);
    close(sockfd);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    int server_socketFD, connect_FD;
    struct sockaddr_in server_addr, cli_addr;
    pthread_t client_p;
    signal(SIGINT, sig_handler);
    if ((server_socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket\t");
        exit(EXIT_FAILURE);
    }
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

    while (!server_flag)
    {
        socklen_t cli_len = sizeof(cli_addr);
        if ((connect_FD = accept(server_socketFD, (struct sockaddr *)&cli_addr, &cli_len)) == -1)
        {
            perror("accept\t");
            continue;
        }
        printf("New client %d\n", connect_FD);

        if ((cli_count += 1) > MAX_CLIENT)
        {
            printf("reach Max ...\n");
            close(connect_FD);
            continue;
        }
        appendNode(&head, connect_FD);
        pthread_create(&client_p, NULL, client_handler, &connect_FD);
    }
    return 0;
}
