#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "RAK3172.h"
#include "freertos/queue.h"
#include "logger.h"

TaskHandle_t xRAK3172ReceiveTaskHandle;

void vMessageProcessTask(void *pvParameters);

// Tag for debug messages
static const char *TAG = "[Receiver]";

// App entrypoint
void app_main(void)
{
    vLoggerTask_Init(10);

    // Initialize WiFi and connect to network
    // wifi_init();
    RAK3172_Init();

    // ESP_LOGI(TAG, "Running...");
    xLoggerTask_Send(TAG, "Running...", portMAX_DELAY);

    // Create task to receive messages from RAK3172
    xTaskCreate(vRAK3172ReceiveTask, "RAK3172_RX", 4096, NULL, 10, &xRAK3172ReceiveTaskHandle);

    // Create task to process received messages
    xTaskCreate(vMessageProcessTask, "PROCESS", 4096, NULL, 10, NULL);

    // Configure P2P settings to match the transmitter and enter continuous RX.
    RAK3172_StartReceiver();
}


void vMessageProcessTask(void *pvParameters)
{
    RAK3172_Message_t msg;

    while (1)
    {
        // 
        if (RAK3172_DequeueMessage(&msg, portMAX_DELAY) == pdPASS)
        {
            // Only for test
            ESP_LOGI("[MAIN]", "RSSI=%d SNR=%d Level=%.2f", msg.rssi, msg.snr, msg.level);
        }
    }
}
