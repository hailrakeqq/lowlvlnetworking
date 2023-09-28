#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

void handleClient(int clientSocket) {
    FILE *mainPage = fopen("res/index.html", "r");
    if(mainPage == -1){
        perror("An error occurred while opening index.html file.");
        return;
    }

    fseek(mainPage, 0, SEEK_END);
    long fileSize = ftell(mainPage);
    fseek(mainPage, 0, SEEK_SET);

    char *fileBuffer = (char *)malloc(fileSize + 1);
    if(fileBuffer == -1){
        perror("Failed to allocate memory to file buffer");
        fclose(mainPage);
        return;
    }

    fread(fileBuffer, 1, fileSize, mainPage);
    fclose(mainPage);

    char response[1024];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n"
                                         "Content-Type: text/html\r\n"
                                         "Content-Length: %ld\r\n"
                                         "Content-Disposition: inline\r\n"
                                         "\r\n"
                                         "%s",
             fileSize, fileBuffer);

    send(clientSocket, response, strlen(response), 0);

    close(clientSocket);
    free(fileBuffer);
}