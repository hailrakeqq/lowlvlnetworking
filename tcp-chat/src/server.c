#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024
#define MAX_USERNAME_SIZE 32
#define MAX_CLIENTS 5

typedef struct {
    int socket;
    struct sockaddr_in address;
    char username[MAX_USERNAME_SIZE];
} client;

client clients[MAX_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

int is_username_taken(const char *username) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].socket != -1 && strcmp(clients[i].username, username) == 0) {
            return 1;
        }
    }
    return 0;
}

char *get_current_time(){
    time_t currentTime;
    struct tm *localTime;
    char *timeString = (char *)malloc(9); // "HH:MM:SS\0"

    if (time(&currentTime) == -1) {
        perror("Error getting current time");
        exit(EXIT_FAILURE);
    }

    localTime = localtime(&currentTime);
    if (localTime == NULL) {
        perror("Error converting time");
        exit(EXIT_FAILURE);
    }

    strftime(timeString, 9, "%H:%M:%S", localTime);

    return timeString;
}

void broadcast_message(char* message, size_t messageSize, int clientIndex){
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (i != clientIndex && clients[i].socket != -1) {
            send(clients[i].socket, message, messageSize, 0);
        }
    }
}

void *handle_client(void *arg) {
    int clientIndex = *((int *)arg);
    int clientSocket = clients[clientIndex].socket;
    char buffer[MAX_BUFFER_SIZE];
    int bytesRead;

    while (1) {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        size_t messageSize = bytesRead + MAX_USERNAME_SIZE;
        char *currentTime = get_current_time();

        if (bytesRead <= 0){
            // client disconected
            pthread_mutex_lock(&mutex);
            printf("(%s) Client disconnected: %s\t%s\n",currentTime, clients[clientIndex].username, inet_ntoa(clients[clientIndex].address.sin_addr));

            sprintf(buffer, "\n(%s) User '%s' has left the chat.\n",currentTime, clients[clientIndex].username);
            broadcast_message(buffer, messageSize, clientIndex);

            pthread_mutex_unlock(&mutex);
            break;
        }

        buffer[bytesRead] = '\0';

        char message[MAX_BUFFER_SIZE + MAX_USERNAME_SIZE];
        sprintf(message, "\n(%s) %s: %s", currentTime, clients[clientIndex].username, buffer);
        message[MAX_BUFFER_SIZE + MAX_USERNAME_SIZE] = '\0';

        pthread_mutex_lock(&mutex);

        broadcast_message(message, messageSize, clientIndex);
    
        pthread_mutex_unlock(&mutex);
    }

    close(clientSocket);
    clients[clientIndex].socket = -1;

    return NULL;
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(struct sockaddr_in);
    pthread_t threads[MAX_CLIENTS];
    int clientIndex;

    // initialize client array
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        clients[i].socket = -1;
    }

    // create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    // configuring server
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding server socket");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        perror("Error listening for clients");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);
        if (clientSocket == -1) {
            perror("Error accepting client connection");
            continue;
        }

        char username[MAX_USERNAME_SIZE];
        memset(username, 0, sizeof(username));
        recv(clientSocket, username, sizeof(username), 0);
        username[strcspn(username, "\n")] = '\0'; // Remove newline character

        // Check if the username is taken
        if (is_username_taken(username)) {
            // Inform the client that the username is taken
            send(clientSocket, "Username taken. Please choose another one.\n", 43, 0);
            close(clientSocket);
            continue;
        }

        pthread_mutex_lock(&mutex);

        for (clientIndex = 0; clientIndex < MAX_CLIENTS; ++clientIndex) {
            if (clients[clientIndex].socket == -1) {
                clients[clientIndex].socket = clientSocket;
                clients[clientIndex].address = clientAddr;

                // Copy the received username to the client structure
                strcpy(clients[clientIndex].username, username);
                break;
            }
        }

        pthread_mutex_unlock(&mutex);

        if (clientIndex == MAX_CLIENTS) {
            printf("Too many clients. Connection rejected: %s\n", inet_ntoa(clientAddr.sin_addr));
            close(clientSocket);
            continue;
        }

        pthread_create(&threads[clientIndex], NULL, handle_client, (void *)&clientIndex);
    }

    close(serverSocket);
    return 0;
}