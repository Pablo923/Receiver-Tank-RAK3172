#ifndef LOGGER_H
#define LOGGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef struct {
    char tag[32];
    char message[128];
} LogMessage_t;

void       vLoggerTask_Init(UBaseType_t uxQueueLength);
BaseType_t xLoggerTask_Send(const char *pcTag, const char *pcMessage, TickType_t xTicksToWait);

#endif