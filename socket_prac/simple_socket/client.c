#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 12345
int *left_flag, sockfd;

void sig_handler(int num)
{
    close(sockfd);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int n, status;
    FILE *fp = stdin;
    char send_buff[BUFFER_SIZE], recv_buff[BUFFER_SIZE];
    struct sockaddr_in address;
    memset(send_buff, 0, BUFFER_SIZE);
    memset(recv_buff, 0, BUFFER_SIZE);
    signal(SIGINT, sig_handler);
    left_flag = mmap(NULL, sizeof(*left_flag), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    (*left_flag) = 0;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket\t");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(address.sin_zero), 8);
    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr)) == -1)
    {
        perror("connect\t");
        exit(1);
    }

    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        perror("fork\t");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    if (pid == 0) // child
    {
        while (!(*left_flag))
        {
            if (fgets(send_buff, BUFFER_SIZE, fp) == NULL)
            {
                *left_flag = 1;
                continue;
            }
            else
            {
                send_buff[strlen(send_buff) - 1] = '\0';
                write(sockfd, send_buff, strlen(send_buff));
                if (strcmp(send_buff, "exit") == 0)
                {
                    *left_flag = 1;
                    break;
                }
            }
            memset(send_buff, 0, BUFFER_SIZE);
        }
        exit(EXIT_SUCCESS);
    }

    if (pid > 0) // parent
    {
        while (!(*left_flag))
        {
            n = read(sockfd, recv_buff, BUFFER_SIZE);
            if (n == 0)
            {
                *left_flag = 1;
                break;
            }
            else if (n == -1)
            {
                perror("read\t");
                *left_flag = 1;
                break;
            }
            else
            {
                recv_buff[strlen(recv_buff)] = '\0';
                printf("%s\n", recv_buff);
            }
            memset(recv_buff, 0, BUFFER_SIZE);
        }
        waitpid(pid, &status, WNOHANG);
    }
    close(sockfd);
    return 0;
}