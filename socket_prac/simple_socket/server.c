#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "singlylinkedlist.c"

#define PORT 12345
int flag = 0, sockfd;
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

void sig_handler(int sig_numb)
{
    close_linkedlist();
    close(sockfd);
    exit(EXIT_FAILURE);
}

int main()
{
    int new_fd;
    struct sockaddr_in my_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    fd_set readfds;
    FD_ZERO(&readfds);

    signal(SIGINT, sig_handler);
    // TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket\t");
        exit(EXIT_FAILURE);
    }

    // Initail, bind port
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // binding
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind\t");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(sockfd, 10) == -1)
    {
        perror("listen\t");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    pid_t pid;
    while (!flag)
    {
        if ((new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
        {
            perror("accept\t");
            close(new_fd);
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        appendNode(&head, new_fd);
        pid = fork();
        if (pid == -1)
        {
            perror("fork\t");
            close(new_fd);
            close(sockfd);
            flag = 1;
            continue;
        }
        else if (pid == 0) // child
        {
            char recv_buff[1024];
            while (!flag)
            {
                FD_SET(new_fd, &readfds);
                if (select(new_fd + 1, &readfds, NULL, NULL, NULL) == 1)
                {
                    memset(recv_buff, 0, sizeof(recv_buff));
                    int ret = read(new_fd, recv_buff, sizeof(recv_buff));
                    printf("%s < %d\n", recv_buff, new_fd);
                    Node *temp = head;
                    while (temp != NULL)
                    {
                        if (temp->data != new_fd)
                        {
                            write(temp->data, recv_buff, strlen(recv_buff));
                        }
                        temp = temp->next;
                    }
                    if (ret == 0 || !strcmp(recv_buff, "exit"))
                    {
                        write(new_fd, "\0", 1);
                        printf("client close: %d\n", new_fd);
                        deleteNode(&head, new_fd);
                        break;
                    }
                    else if (ret == -1)
                    {
                        perror("recv \t");
                        break;
                    }
                }
            }
            close(new_fd);
            exit(EXIT_SUCCESS);
        }
    }
    close_linkedlist();
    close(sockfd);
    return 0;
}