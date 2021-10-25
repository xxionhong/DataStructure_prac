#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()

{
    int sockfd, new_fd;
    char message[] = {"Message from server!\n"};
    char buf[2000] = "";
    struct sockaddr_in my_addr;
    
    // TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
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
        perror("bind");
        exit(1);
    }

    // Start listening
    if (listen(sockfd, 10) == -1)
    {
        perror("listen");
        exit(1);
    }

    // Connect

    
    if ((new_fd = accept(sockfd, NULL, NULL)) == -1)
    {
        perror("accept");
        exit(1);
    }
    
    write(new_fd, message, sizeof(message));
    read(new_fd, buf, sizeof(buf));
    printf("%s", buf);
    /*
    close(new_fd);
    close(sockfd);
    */

    while (new_fd)
    {
        pid_t pid;
        if ((pid = fork()) == 0)
        {
            printf("Fork PID= %d\n", getpid());
            while (recv(new_fd, buf, sizeof(buf), 0) > 0)
            {
                printf("Message Received: %s\n", buf);
                if (!strcmp(buf, "exit"))
                {
                    close(new_fd);
                    printf("Kill Pid: %d\n", getpid());
                    kill(getpid(),SIGKILL);
                }
                memset(buf, 0, sizeof(buf));
            }
            close(new_fd);
            exit(0);
        }
    }
    return 0;
}