#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_CLIENT 10
#define BUFFER_SIZE 1024
#define PORT 12345
#define REACH_MAXIMUM "reach_maximum"
#define SERVER_LEFT "server_left"
int cli_count = 0, uuid_init = 100;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct client_info
{
    char name[21];
    int socketFD;
    int uuid;
} cli_info;

cli_info *cli[MAX_CLIENT];

// cli[MAX_CLIENT] control (enqueue/dequeue)
void queue_manage(cli_info *ci, int state)
{
    pthread_mutex_lock(&queue_mutex);
    switch (state)
    {
    case 0:
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            if (!cli[i])
            {
                cli[i] = ci;
                cli_count += 1;

                break;
            }
        }
        break;
    case 1:
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            if (cli[i])
            {
                if (cli[i]->uuid == ci->uuid)
                {
                    cli[i] = NULL;
                    cli_count -= 1;
                    break;
                }
            }
        }
        break;
    }
    printf("server count: %d\n", cli_count);
    pthread_mutex_unlock(&queue_mutex);
}

// after server received a message, send to all clients expect itself
void send_to_clients(int uid, char *send_buff)
{
    pthread_mutex_lock(&queue_mutex);
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (cli[i])
        {
            if (cli[i]->uuid != uid)
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

// for pthread clients
void *client_handler(void *client_in)
{
    char recv_buff[BUFFER_SIZE];
    int leave_f = 0;

    cli_info *client = (cli_info *)client_in;

    read(client->socketFD, recv_buff, BUFFER_SIZE + 21);
    strcpy(client->name, recv_buff);
    printf("new client %s\n", client->name);
    strcat(recv_buff, " < ");
    strcat(recv_buff, client->name);
    send_to_clients(client->uuid, recv_buff);
    memset(recv_buff, 0, BUFFER_SIZE);
    while (1)
    {
        if (leave_f)
        {
            break;
        }
        int receive = read(client->socketFD, recv_buff, BUFFER_SIZE);
        if (receive > 0)
        {
            if (strlen(recv_buff) > 0)
            {
                printf("%s\n", recv_buff);
                send_to_clients(client->uuid, recv_buff);
            }
        }
        else if (receive == 0 || !strcmp(recv_buff, "exit"))
        {
            printf("%s leave!\n", client->name);
            send_to_clients(client->uuid, recv_buff);
            leave_f = 1;
        }
        else
        {
            leave_f = 1;
        }
        memset(recv_buff, 0, BUFFER_SIZE);
    }
    queue_manage(client, 1);

    close(client->socketFD);
    pthread_exit(NULL);
}
// close all client fd after server left
void close_all_fd()
{
    pthread_mutex_lock(&queue_mutex);
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (cli[i])
        {
            send(cli[i]->socketFD, SERVER_LEFT, sizeof(SERVER_LEFT), 0);
            close(cli[i]->socketFD);
        }
    }
    pthread_mutex_unlock(&queue_mutex);
}

// handling signal SIGINT
void sig_handler(sig_atomic_t sig_num)
{
    printf("Catch ctrl + c, SIG_NUM=%d\n", sig_num);
    close_all_fd();
    sleep(2);
    printf("\e[1;1H\e[2J");
    exit(EXIT_SUCCESS);
}

// for server function
void *server_handler()
{
    while (1)
    {
        int input_content;
        scanf("%d", &input_content);
        switch (input_content)
        {
        case 0:
            pthread_mutex_lock(&queue_mutex);
            for (int i = 0; i < MAX_CLIENT; i++)
            {
                if (cli[i])
                {
                    printf("Name: %10s, uuid:%d, socketfd: %d\n", cli[i]->name, cli[i]->uuid, cli[i]->socketFD);
                }
            }
            pthread_mutex_unlock(&queue_mutex);
            break;
        case 9:
            close_all_fd();
            printf("Server exit!\n");
            kill(getpid(), SIGINT);
            break;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    int server_socketFD, connect_FD;
    struct sockaddr_in server_addr;
    pthread_t client_p, server_p;
    memset(cli, 0, sizeof(cli_info) * MAX_CLIENT);
    signal(SIGINT, sig_handler); // catch interrupt
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
    pthread_create(&server_p, NULL, &server_handler, NULL);
    while (1)
    {
        connect_FD = accept(server_socketFD, NULL, NULL);
        if ((cli_count + 1) == MAX_CLIENT)
        {
            printf("%s\n", REACH_MAXIMUM);
            send(connect_FD, REACH_MAXIMUM, sizeof(REACH_MAXIMUM), 0);
            close(connect_FD);
            continue;
        }
        cli_info *cli_temp = (cli_info *)malloc(sizeof(cli_info));
        cli_temp->socketFD = connect_FD;
        cli_temp->uuid = uuid_init++;
        queue_manage(cli_temp, 0);
        pthread_create(&client_p, NULL, &client_handler, (void *)cli_temp);
        sleep(2);
    }
    close(server_socketFD);
    close(connect_FD);
    pthread_mutex_destroy(&queue_mutex);

    return 0;
}
