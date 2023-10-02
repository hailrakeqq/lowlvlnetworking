#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

static FILE *router(char *pathToFile) {
    char filePath[128];
    snprintf(filePath, sizeof(filePath), "res%s.html", pathToFile);

    FILE *file = fopen(filePath, "r");
    if(file == NULL){
        if(strcmp(pathToFile, "/") == 0)
            file = fopen("res/index.html", "r");
        else
            file = fopen("res/notFoundPage.html", "r");
    }

    return file;
}

static void sendResponse(int clientSocket, char* fileBuffer, long fileSize){
    char response[1024];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n"
                                         "Content-Type: text/html\r\n"
                                         "Content-Length: %ld\r\n"
                                         "Content-Disposition: inline\r\n"
                                         "\r\n"
                                         "%s",
             fileSize, fileBuffer);

    send(clientSocket, response, strlen(response), 0);
}

void* handleClient(int clientSocket, char *method, char *path)
{
    FILE *filePage = router(path);

    fseek(filePage, 0, SEEK_END);
    long fileSize = ftell(filePage);
    fseek(filePage, 0, SEEK_SET);

    char *fileBuffer = (char *)malloc(fileSize + 1);
    if(fileBuffer == NULL){
        perror("Failed to allocate memory to file buffer");
        fclose(filePage);
        return NULL;
    }

    fread(fileBuffer, 1, fileSize, filePage);
    fclose(filePage);

    sendResponse(clientSocket, fileBuffer, fileSize);

    close(clientSocket);
    free(fileBuffer);
}