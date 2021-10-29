#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 12345
#define STDIN 0

void set_nonblock(int fd)
{
    int flags;
    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        perror("fcntl\t");
        exit(EXIT_FAILURE);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        perror("fcntl\t");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char const *argv[])
{
    char recv_buff[BUFFER_SIZE], send_buff[BUFFER_SIZE];
    struct sockaddr_in conn_info;
    int sockfd, left_flag = 0;
    memset(recv_buff, 0, BUFFER_SIZE);
    memset(send_buff, 0, BUFFER_SIZE);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket\t");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    // set_nonblock(sockfd);
    conn_info.sin_family = AF_INET;
    conn_info.sin_port = htons(PORT);
    conn_info.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(conn_info.sin_zero), 0, sizeof(conn_info.sin_zero));

    if (connect(sockfd, (struct sockaddr *)&conn_info, sizeof(struct sockaddr)) == -1)
    {
        perror("connect\t");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    set_nonblock(sockfd);
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(sockfd, &readfds);
    struct timeval timeout = {2, 0};
    if (select(sockfd + 1, &readfds, NULL, NULL, &timeout) == -1)
    {
        perror("select\t");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (!left_flag)
    {
        scanf("%s", send_buff);
        if (FD_ISSET(sockfd, &readfds))
        {
            int temp = read(sockfd, recv_buff, strlen(recv_buff));
            if (temp > 0)
            {
                printf("%s\n>", recv_buff);
            }
            else if (temp == 0)
            {
                left_flag = 1;
                break;
            }
            memset(recv_buff, 0, BUFFER_SIZE);
        }
        else
        {
            FD_CLR(sockfd, &readfds);
            write(sockfd, send_buff, strlen(send_buff));
            if (!strcmp(send_buff, "exit"))
            {
                left_flag = 1;
                break;
            }
            memset(send_buff, 0, BUFFER_SIZE);
        }
    }
    close(sockfd);

    return 0;
}
