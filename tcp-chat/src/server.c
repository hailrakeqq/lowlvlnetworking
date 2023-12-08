#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_CLIENTS 10

int create_socket(struct sockaddr_in *server_addr){
    //create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    //setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    server_addr->sin_addr.s_addr = INADDR_ANY;

    //bind socket
    if(bind(server_socket, (struct sockaddr_in* )&server_addr, sizeof(server_addr)) == -1){
        perror("Error binding");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

}

void handleClient(int client_socket){
    char buffer[1024];
    int bytes_received;
    
    while(1){
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        buffer[bytes_received] = '\0';
        printf("Bytes received from client: %s", buffer);

        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
}

int main(){
    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int server_socket = create_socket(&server_addr);

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error listening");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while(1){ 
        //accept connection from a client
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

        //create new process to handle the client
        if(fork() == 0){
            close(server_socket);
            handleClient(client_socket);
            exit(EXIT_SUCCESS);
        } else {
            close(client_socket);
        }
    }

    return 0;
}