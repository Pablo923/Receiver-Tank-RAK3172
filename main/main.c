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

// Tag for debug messages
static const char *TAG = "[Receiver]";

// App entrypoint
void app_main(void)
{
    vLoggerTask_Init(10);

    // Initialize WiFi and connect to network
    // wifi_init();
    init_RAK3172();

    // ESP_LOGI(TAG, "Running...");
    xLoggerTask_Send(TAG, "Running...", portMAX_DELAY);

    // Create Task to Print RAK3172 Responses
    xTaskCreate(uartRAK3172_receiveTask, "uartRAK3172_receiveTask", 2048, NULL, 10, NULL);
    
    // Set RAK3172 as Receiver
    RAK3172_sendCommand("AT+PRECV=65534");
    vTaskDelay(1000 / portTICK_PERIOD_MS); 

    while(1) 
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS); 
    }
}
