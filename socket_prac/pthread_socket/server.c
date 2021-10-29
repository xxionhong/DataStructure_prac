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

#define MAX_CLIENT 10
#define BUFFER_SIZE 1024
#define PORT 12345

int cli_count = 0, server_flag = 0;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct client_info
{
    char name[21];
    int socketFD;
} cli_info;

cli_info *cli[MAX_CLIENT];

// cli[MAX_CLIENT] control (enqueue/dequeue)
void queue_manage(cli_info *ci, int stats)
{
    pthread_mutex_lock(&queue_mutex);
    switch (stats)
    {
    case 0:
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            if (!cli[i])
            {
                cli[i] = ci;
                cli_count += 1;
                printf("server count: %d\n", cli_count);
                break;
            }
        }
        break;
    case 1:
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            if (cli[i])
            {
                if (cli[i]->socketFD == ci->socketFD)
                {
                    cli[i] = NULL;
                    cli_count -= 1;
                    printf("server count: %d\n", cli_count);
                    break;
                }
            }
        }
        break;
    }
    pthread_mutex_unlock(&queue_mutex);
}

// after server received a message, send to all clients expect itself
void send_to_clients(int fd, char *send_buff)
{
    pthread_mutex_lock(&queue_mutex);
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (cli[i])
        {
            if (cli[i]->socketFD != fd)
            {
                if (send(cli[i]->socketFD, send_buff, strlen(send_buff), 0) < 0)
                {
                    perror("write\t");
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&queue_mutex);
}

void close_all_fd()
{
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (cli[i])
        {
            close(cli[i]->socketFD);
        }
    }
}

// for pthread clients
void *client_handler(void *client_in)
{
    char recv_buff[BUFFER_SIZE];
    int client_flag = 0;

    cli_info *client = (cli_info *)client_in;

    read(client->socketFD, recv_buff, BUFFER_SIZE);
    strcpy(client->name, recv_buff);
    printf("new client %s\n", client->name);
    strcat(recv_buff, " < ");
    strcat(recv_buff, client->name);
    send_to_clients(client->socketFD, recv_buff);
    memset(recv_buff, 0, BUFFER_SIZE);
    while (!client_flag)
    {
        if (client_flag)
        {
            break;
        }
        int receive = read(client->socketFD, recv_buff, BUFFER_SIZE);
        if (receive > 0)
        {
            if (strlen(recv_buff) > 0)
            {
                printf("%s\n", recv_buff);
                send_to_clients(client->socketFD, recv_buff);
            }
        }
        else if (receive == 0 || !strcmp(recv_buff, "exit"))
        {
            printf("%s leave!\n", client->name);
            send_to_clients(client->socketFD, recv_buff);
            client_flag = 1;
        }
        else
        {
            client_flag = 1;
        }
        memset(recv_buff, 0, BUFFER_SIZE);
    }
    queue_manage(client, 1);
    free(client);
    pthread_exit(NULL);
}

void sig_handler(int a)
{
    server_flag = 1;
    close_all_fd();
    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[])
{
    int server_socketFD, connect_FD;
    struct sockaddr_in server_addr;
    pthread_t client_p;
    memset(cli, 0, sizeof(cli_info) * MAX_CLIENT);
    signal(SIGINT, sig_handler);
    if ((server_socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket\t");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

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
        connect_FD = accept(server_socketFD, NULL, NULL);
        if ((cli_count + 1) == MAX_CLIENT)
        {
            printf("MAX_CLIENT reached\n");
            close(connect_FD);
            continue;
        }
        else if (connect_FD != 0)
        {
            cli_info *cli_temp = (cli_info *)malloc(sizeof(cli_info));
            cli_temp->socketFD = connect_FD;
            queue_manage(cli_temp, 0);
            pthread_create(&client_p, NULL, &client_handler, (void *)cli_temp);
            pthread_join(client_p, NULL);
            free(cli_temp);
        }
    }
    close(server_socketFD);
    close_all_fd();
    pthread_mutex_destroy(&queue_mutex);
    printf("\e[1;1H\e[2J");
    return 0;
}
