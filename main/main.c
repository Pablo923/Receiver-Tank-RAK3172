#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"

#include "wifi.h"

// Settings
static const uint32_t sleep_time_ms = 1000;

// Tag for debug messages
static const char *TAG = "[Receiver]";

// App entrypoint
void app_main(void)
{
    // Initialize WiFi and connect to network
    wifi_init();

    // Superloop
    while (1) {
        
        // Read event group bits to check WiFi connection status
        EventBits_t bits = xEventGroupGetBits(network_event_group);

        if (bits & WIFI_STA_CONNECTED_BIT) {
            ESP_LOGI(TAG, "Still connected to WiFi network");
        } else {
            ESP_LOGE(TAG, "Lost connection to network - reconnecting...");
            wifi_sta_reconnect();
        }

        vTaskDelay(sleep_time_ms / portTICK_PERIOD_MS);
    }
}
