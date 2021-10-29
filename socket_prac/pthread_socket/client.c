#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 12345

int socketFD;
char name_buff[21];
int flag = 0;

void *recv_handler()
{
    char recv_buff[BUFFER_SIZE];
    while (!flag)
    {
        int tmp = recv(socketFD, recv_buff, sizeof(recv_buff), 0);
        if (tmp > 0)
        {
            printf("%s\n>", recv_buff);
        }
        else if (tmp == 0)
        {
            flag = 1;
            break;
        }
        memset(recv_buff, 0, BUFFER_SIZE);
    }
    pthread_exit(NULL);
}
void *send_handler()
{
    char send_buff[BUFFER_SIZE + 21];
    while (!flag)
    {
        printf("Please input data (exit)\n>");
        scanf("%s", send_buff);
        if (!strcmp(send_buff, "exit"))
        {
            flag = 1;
        }
        strcat(send_buff, " < ");
        strcat(send_buff, name_buff);
        send(socketFD, send_buff, BUFFER_SIZE, 0);

        memset(send_buff, 0, BUFFER_SIZE + 21);
    }
    pthread_exit(NULL);
}
int main(int argc, char const *argv[])
{

    struct sockaddr_in conn_info;
    if ((socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket\t");
        exit(EXIT_FAILURE);
    }
    conn_info.sin_family = AF_INET;
    conn_info.sin_port = htons(PORT);
    conn_info.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(conn_info.sin_zero), 0, sizeof(conn_info.sin_zero));

    if (connect(socketFD, (struct sockaddr *)&conn_info, sizeof(struct sockaddr)) == -1)
    {
        perror("connect\t");
        exit(EXIT_FAILURE);
    }
    printf("Please input a Name (<20 Char)\n>");
    scanf("%20s", name_buff);
    send(socketFD, name_buff, sizeof(name_buff), 0);

    pthread_t recv_handler_t, send_handler_t;
    if ((pthread_create(&recv_handler_t, NULL, (void *)&recv_handler, NULL) != 0) ||
        (pthread_create(&send_handler_t, NULL, (void *)&send_handler, NULL) != 0))
    {
        perror("handler\t");
        close(socketFD);
        exit(EXIT_FAILURE);
    }
    pthread_join(send_handler_t, NULL);
    close(socketFD);
    printf("\e[1;1H\e[2J");
    return 0;
}
