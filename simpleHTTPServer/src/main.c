#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../include/logger.h"

#define MAX_CLIENTS 5

extern void* handleClient(int clientSocket, char *method, char *path);

struct ThreadArgs{
    int clientSocket;
    char *method;
    char *path;
};

void *ThreadEntryPoint(void *arg) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)arg;

    int clientSocket = threadArgs->clientSocket;
    char *method = threadArgs->method;
    char *path = threadArgs->path;
    handleClient(clientSocket, method, path);

    free(threadArgs);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    pthread_t threads[MAX_CLIENTS];
    int threadCount = 0;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_port = htons(2096);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    listen(serverSocket, 5);

    printf("Server listening 2096 port.\n");
    while(1){
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if(clientSocket == -1){
            perror("Accept failed");
            exit(1);
        }

        char request[256];
        recv(clientSocket, request, sizeof(request), 0);
        char method[16];
        char path[128]; 
        sscanf(request, "%s %s", method, path);
        
        struct ThreadArgs *args = malloc(sizeof(struct ThreadArgs));
        args->clientSocket = clientSocket;
        args->method = method;
        args->path = path;

        if(pthread_create(&threads[threadCount], NULL, ThreadEntryPoint, args) != 0){
            perror("Thread creation failed");
            free(args);
            close(clientSocket);
        } else {
            threadCount++;
        }
    }

    close(serverSocket);
    return 0;
}