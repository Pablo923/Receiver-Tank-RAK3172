#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "logger.h"

#define LOGGER_STACK_SIZE  2048
#define LOGGER_PRIORITY    5

static QueueHandle_t xLoggerQueue     = NULL;
static TaskHandle_t  xLoggerTaskHandle = NULL;

static void prvLoggerTask(void *pvParameters)
{
    LogMessage_t xReceivedMsg;

    for (;;)
    {
        if (xQueueReceive(xLoggerQueue, &xReceivedMsg, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(xReceivedMsg.tag, "%s", xReceivedMsg.message);
        }
    }
}

void vLoggerTask_Init(UBaseType_t uxQueueLength)
{
    xLoggerQueue = xQueueCreate(uxQueueLength, sizeof(LogMessage_t));
    configASSERT(xLoggerQueue != NULL);

    xTaskCreate(prvLoggerTask, "LoggerTask", LOGGER_STACK_SIZE, NULL, LOGGER_PRIORITY, &xLoggerTaskHandle);
    configASSERT(xLoggerTaskHandle != NULL);
}

BaseType_t xLoggerTask_Send(const char *pcTag, const char *pcMessage, TickType_t xTicksToWait)
{
    LogMessage_t xMsg;
    snprintf(xMsg.tag,     sizeof(xMsg.tag),     "%s", pcTag);
    snprintf(xMsg.message, sizeof(xMsg.message), "%s", pcMessage);
    return xQueueSend(xLoggerQueue, &xMsg, xTicksToWait);
}