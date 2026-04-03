#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "wifi.h"

// Tag for debug messages
static const char *TAG = "[Receiver]";

// App entrypoint
void app_main(void)
{
    // Initialize WiFi and connect to network
    wifi_init();

    while(1) 
    {
        ESP_LOGI(TAG, "Receiver is running...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
