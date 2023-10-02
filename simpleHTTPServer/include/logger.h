#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

typedef enum
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} logLevel;

void loggerLog(logLevel logLevel, char* filePath, char* message);

#endif // LOGGER_H
