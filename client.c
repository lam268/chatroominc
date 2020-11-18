#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_CLIENT 100
#define BUFFER_SIZE 2048

volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];

void str_overwrite_stdout()
{
    printf("\r%s", "> ");
    fflush(stdout);
}

void str_trim_lf(char *arr, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (arr[i] == '\n')
        {
            arr[i] = '\0';
            break;
        }
    }
}

void catch_ctrl_c_and_exit()
{
    flag = 1;
}

void recv_msg_handler()
{
    char message[BUFFER_SIZE] = {};
    while (1)
    {
        int receive = recv(sockfd, message, BUFFER_SIZE, 0);
        if (receive > 0)
        {
            printf("%s ", message);
            str_overwrite_stdout();
        }
        else if (receive == 0)
        {
            break;
        }
        bzero(message, BUFFER_SIZE);
    }
}

void send_msg_handler()
{
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE + 32] = {};

    while (1)
    {
        str_overwrite_stdout();
        fgets(buffer, BUFFER_SIZE, stdin);

        str_trim_lf(buffer, BUFFER_SIZE);

        if (strcmp(buffer, "exit") == 0)
        {
            break;
        }
        else
        {
            sprintf(message, "%s: %s\n", name, buffer);
            send(sockfd, message, strlen(message), 0);
        }

        bzero(buffer, BUFFER_SIZE);
        bzero(message, BUFFER_SIZE + 32);
    }

    catch_ctrl_c_and_exit(2);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage:  %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);
    int listenfd;

    signal(SIGINT, catch_ctrl_c_and_exit);

    printf("Enter your name\n");
    fgets(name, 32, stdin);
    str_trim_lf(name, strlen(name));

    if (strlen(name) > 32 - 1 || strlen(name) < 2)
    {
        printf("Enter your name correctly\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;

    /* Socket settings */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    // Connect to Server
    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("ERROR: connect\n");
        return EXIT_FAILURE;
    }

    // Send name
    send(sockfd, name, 32, 0);

    printf("=== WELCOME TO THE CHATROOM ===\n");

    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void *)send_msg_handler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    while (1)
    {
        if (flag)
        {
            printf("Bye\n");
            break;
        }
    }

    close(sockfd);
    return EXIT_SUCCESS;
}