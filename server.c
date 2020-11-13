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
#define SO_REUSEPORT 15
#define BUFFER_SIZE 2048

static _Atomic unsigned int cli_count = 0;
static int uid = 10;

int main (int argc, char *argv[1]) {
    if (argc != 2) {
        printf("Usage: %s <port\n>",argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);

    int option = 1;
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    pthread_t pid;

    //Socket Settings:

    listenfd = socket (AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port);

    signal(SIGPIPE, SIG_IGN);

    if (setsockopt(listenfd, SOL_SOCKET, (SO_REUSEADDR | SO_REUSEPORT), (char*)&option, sizeof(option)) < 0){
        printf("ERROR: setsockopt\n");
        return EXIT_FAILURE;
    }

    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Error: bind\n");
        return EXIT_FAILURE;
    }

    if (listen(listenfd, 10) < 0){
        printf("Error: listen\n");
        return EXIT_FAILURE;
    }

    printf("=== Welcome to Chat room ==\n");

    while (1) {
        socklen_t clilen = sizeof(cli_addr);
        connfd = accept(listenfd, (struct sockaddr *)&cli_addr, clilen);
    }

    if ((cli_count + 1) == MAX_CLIENT){
        printf("Maximum clients connected. Connection Rejected!!!");
        print_ip_addr(cli_addr);
        close(connfd);
        return EXIT_FAILURE;
    }

}
