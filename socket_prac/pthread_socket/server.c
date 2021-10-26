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

int cli_count = 0, uuid_init = 100;

pthread_mutex_t queue_mutex;

typedef struct client_info
{
    char name[21];
    int socketFD;
    int uuid;
} cli_info;

cli_info *cli[MAX_CLIENT];

// cli[MAX_CLIENT] control (enqueue/dequeue)
void enqueue(cli_info *ci)
{
    pthread_mutex_lock(&queue_mutex);
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
    pthread_mutex_unlock(&queue_mutex);
    return;
}
void dequeue(int uid)
{
    pthread_mutex_lock(&queue_mutex);
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (cli[i])
        {
            if (cli[i]->uuid == uid)
            {
                cli[i] = NULL;
                cli_count -= 1;
                printf("server count: %d\n", cli_count);
                break;
            }
        }
    }
    pthread_mutex_unlock(&queue_mutex);
    return;
}

// after server received a message, send to all clients expect itself
void send_to_clients(int uid, char *send_buff)
{
    pthread_mutex_lock(&queue_mutex);
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (cli[i]->uuid != uid)
        {
            if (write(cli[i]->socketFD, send_buff, sizeof(send_buff)) < 0)
            {
                perror("write\t");
                break;
            }
        }
    }
    pthread_mutex_unlock(&queue_mutex);
}

// for pthread clients
void *client_handler(void *client_in)
{
    char recv_buff[BUFFER_SIZE];
    cli_info *client = (cli_info *)client_in;
    read(client->socketFD, recv_buff, BUFFER_SIZE);
    strcpy(client->name, recv_buff);
    printf("new client %s\n", client->name);
    while (recv(client->socketFD, recv_buff, BUFFER_SIZE, 0) > 0)
    {
        if (sizeof(recv_buff) > 0)
        {
            printf("%s(%d) -> %s\n", client->name, client->uuid, recv_buff);
            send_to_clients(client->uuid, recv_buff);
            if (!strcmp(recv_buff, "exit"))
            {
                printf("%s leave!\n", client->name);
                dequeue(client->uuid);
                break;
            }
            memset(recv_buff, 0, BUFFER_SIZE);
        }
    }
    close(client->socketFD);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    int server_socketFD, connect_FD;
    struct sockaddr_in server_addr;
    // char name_buff[21];
    pthread_t client_p;
    memset(cli, 0, sizeof(cli_info) * MAX_CLIENT);
    pthread_mutex_init(&queue_mutex, NULL);

    if ((server_socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket\t");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    if (bind(server_socketFD, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1 || listen(server_socketFD, MAX_CLIENT) == -1)
    {
        perror("Error:\t");
        close(server_socketFD);
        exit(EXIT_FAILURE);
    }
    printf("Starting Server Side Function...\n");

    while (1)
    {
        connect_FD = accept(server_socketFD, NULL, NULL);
        if ((cli_count + 1) == MAX_CLIENT)
        {
            printf("MAX_CLIENT reached\n");
            close(connect_FD);
            continue;
        }
        // read(connect_FD, name_buff, sizeof(name_buff));
        cli_info *cli_temp = (cli_info *)malloc(sizeof(cli_info));
        cli_temp->socketFD = connect_FD;
        cli_temp->uuid = uuid_init++;
        // strcpy(cli_temp->name, name_buff);
        // printf("New client %s\n", cli_temp->name);
        enqueue(cli_temp);
        pthread_create(&client_p, NULL, &client_handler, (void *)cli_temp);
        free(cli_temp);
    }
    close(server_socketFD);
    close(connect_FD);
    pthread_mutex_destroy(&queue_mutex);
    return 0;
}
