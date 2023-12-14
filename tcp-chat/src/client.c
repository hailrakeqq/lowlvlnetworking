// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <pthread.h>

// #define SERVER_IP "127.0.0.1"
// #define PORT 12345
// #define MAX_BUFFER_SIZE 1024
// #define MAX_USERNAME_SIZE 32

// void *receiveMessages(void *arg) {
//     int clientSocket = *((int *)arg);
//     char buffer[MAX_BUFFER_SIZE];
//     int bytesRead;

//     while (1) {
//         bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
//         if (bytesRead <= 0) {
//             printf("Disconnected from the server.\n");
//             break;
//         }

//         buffer[bytesRead] = '\0';
//         printf("%s", buffer);

//         if (strstr(buffer, "Username taken. Please choose another one.") != NULL) {

//             close(clientSocket);
//             goto restart;
//             // add goto here

//             // char newUsername[MAX_USERNAME_SIZE];
//             // printf("Enter a new username: ");
//             // fgets(newUsername, sizeof(newUsername), stdin);
//             // newUsername[strcspn(newUsername, "\n")] = '\0';
            
//             // send(clientSocket, newUsername, strlen(newUsername), 0);
//         }
//     }

//     pthread_exit(NULL);
// }

// int main() {
//     int clientSocket;
//     struct sockaddr_in serverAddr;
//     pthread_t receiveThread;

//     reconnect:
//     clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (clientSocket == -1) {
//         perror("Error creating client socket");
//         exit(EXIT_FAILURE);
//     }

//     memset(&serverAddr, 0, sizeof(serverAddr));
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP); 
//     serverAddr.sin_port = htons(PORT);

//     if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
//         perror("Error connecting to server");
//         exit(EXIT_FAILURE);
//     }

//     printf("Connected to server on port %d\n", PORT);

//     pthread_create(&receiveThread, NULL, receiveMessages, (void *)&clientSocket);

//     char username[32];
//     memset(username, 0, sizeof(username));
//     printf("Enter your username: ");
//     fgets(username, sizeof(username), stdin);
//     username[strcspn(username, "\n")] = '\0'; 

//     send(clientSocket, username, strlen(username), 0);

//     char message[MAX_BUFFER_SIZE];

//     while (1) {
//         fgets(message, sizeof(message), stdin);

//         if (strcmp(message, "exit\n") == 0) 
//             break;

//         send(clientSocket, message, strlen(message), 0);
//     }

//     close(clientSocket);
//     pthread_join(receiveThread, NULL);

//     return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define MAX_BUFFER_SIZE 1024
#define MAX_USERNAME_SIZE 32

void *receiveMessages(void *arg) {
    int clientSocket = *((int *)arg);
    char buffer[MAX_BUFFER_SIZE];
    int bytesRead;

    while (1) {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            printf("Disconnected from the server.\n");
            break;
        }

        buffer[bytesRead] = '\0';
        printf("%s", buffer);

        if (strstr(buffer, "Username taken. Please try to reconnect and choose another one.") != NULL) {
            close(clientSocket);
            return NULL;  // Exit the thread
        }
    }

    pthread_exit(NULL);
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    pthread_t receiveThread;

    while (1) {
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            perror("Error creating client socket");
            exit(EXIT_FAILURE);
        }

        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
        serverAddr.sin_port = htons(PORT);

        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("Error connecting to server");
            close(clientSocket);
            // Sleep for a short time before attempting to reconnect
            sleep(1);
            continue;  // Restart the loop
        }

        printf("Connected to server on port %d\n", PORT);

        pthread_create(&receiveThread, NULL, receiveMessages, (void *)&clientSocket);

        char username[MAX_USERNAME_SIZE];
        memset(username, 0, sizeof(username));
        printf("Enter your username: ");
        fgets(username, sizeof(username), stdin);
        username[strcspn(username, "\n")] = '\0';

        send(clientSocket, username, strlen(username), 0);

        char message[MAX_BUFFER_SIZE];

        while (1) {
            fgets(message, sizeof(message), stdin);

            if (strcmp(message, "exit\n") == 0)
                break;

            send(clientSocket, message, strlen(message), 0);
        }

        close(clientSocket);
        pthread_join(receiveThread, NULL);
        break;  // Exit the loop
    }

    return 0;
}