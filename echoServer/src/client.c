#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(){
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket == -1){
        perror("An error occurred while creating socket");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(12345);

    if(connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("An error occurred while connecting to the server");
        exit(1);
    }

    printf("You connected to the server\n");
    
    while(1){
        printf("Enter message(enter CTR^C to exit): ");
        fgets(buffer, sizeof(buffer), stdin);

        ssize_t bytesSent = send(clientSocket, buffer, strlen(buffer), 0);
        if(bytesSent == -1){
            perror("An error occurred while sending message to the server");
            break;
        }

        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if(bytesReceived == -1){
            perror("An error occurred while received message from server");
            break;
        }

        if(bytesReceived == 0) {
            printf("Server has been disconnected");
            break;
        }

        buffer[bytesReceived] = '\0';
        printf("Server sent: %s", buffer);
    }

    close(clientSocket);

    return 0;
}