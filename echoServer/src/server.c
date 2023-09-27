#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main() {
  int serverSocket, clientSocket;
  struct sockaddr_in serverAddr, clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  char buffer[1024];

  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(serverSocket == -1){
    perror("Error was occurred while creating socket");
    exit(1);
  }

  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(12345);

  if(bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1){
    perror("Error occurred while binding...");
    exit(1);
  }

  if(listen(serverSocket, 5) == -1){
    perror("Error occurred while listening socket");
    exit(1);
  }
  while(1){
    printf("Echo server waiting for connection...\n");

    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if(clientSocket == -1){
      perror("Error occurred while accepting connection");
      exit(1);
    }

    printf("Client connected\n");

    while(1) {
      ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

      if(bytesReceived == -1){
        perror("An error occurred while receiving data from the client\n");
        break;
      }

      if(bytesReceived == 0){
        printf("Client disconnect\n");
        break;
      }

      ssize_t bytesSent = send(clientSocket, buffer, bytesReceived, 0);
      if(bytesSent == -1){
        perror("An error occurred while sending data to the client");
        break;
      }
      buffer[bytesReceived] = '\0';
      printf("Client Sent: %s\n", buffer);
    }
  }

  close(clientSocket);
  close(serverSocket);

  return 0;
}
