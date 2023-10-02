#include "../include/logger.h"
char* getLogLvlAsString(logLevel logLevel){
    char *logLvl;

    switch (logLevel)
    {
    case 0:
        logLvl = "LOG_INFO";
        break;
    case 1:
        logLvl = "LOG_WARNING";
        break;
    case 2:
        logLvl = "LOG_ERROR";
        break;
    }

    return logLvl;
}

void loggerLog(logLevel logLevel, char* filePath, char* message){
    time_t currentTime;
    struct tm *localTime;
    char dateTime[20];

    time(&currentTime);
    localTime = localtime(&currentTime);
    
    strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", localTime);
    char *logLvl = getLogLvlAsString(logLevel);

    FILE *logFile = fopen(filePath, "a");
    if (logFile == NULL) {
        printf("An error occurred while opening log-file.\n");
        return;
    }
    
    fprintf(logFile, "\n[%s][%s]: %s\n", dateTime, logLvl, message);
    fclose(logFile);
}