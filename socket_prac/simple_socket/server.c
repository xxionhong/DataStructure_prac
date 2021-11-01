#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>

int flag = 0;

void sig_handler(int sig_numb)
{
    flag = 1;
    exit(1);
}

int main()
{
    int sockfd, new_fd;
    struct sockaddr_in my_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    fd_set readfds;
    FD_ZERO(&readfds);

    signal(SIGINT, sig_handler);
    // TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket\t");
        exit(1);
    }

    // Initail, bind port
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(12345);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(my_addr.sin_zero), 8);

    // binding
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind\t");
        close(sockfd);
        exit(1);
    }

    // Start listening
    if (listen(sockfd, 10) == -1)
    {
        perror("listen\t");
        close(sockfd);
        exit(1);
    }
    pid_t pid;
    while (!flag)
    {
        if ((new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
        {
            perror("accept\t");
            close(new_fd);
            close(sockfd);
            exit(1);
        }
        pid = fork();
        if (pid == -1)
        {
            perror("fork\t");
            close(new_fd);
            close(sockfd);
            exit(1);
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
                    printf("Sockfd %d: %s\n", new_fd, recv_buff);
                    if (ret == 0 || !strcmp(recv_buff, "exit"))
                    {
                        write(new_fd, "\0", 1);
                        printf("client close: %d\n", new_fd);
                        break;
                    }
                    else if (ret == -1)
                    {
                        perror("recv \t");
                        break;
                    }
                }
            }
            flag = 1;
            close(new_fd);
        }
        else
        {
            continue;
        }
    }
    flag = 1;
    close(sockfd);
    return 0;
}