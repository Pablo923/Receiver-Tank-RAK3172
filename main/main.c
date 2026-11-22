#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "wifi.h"
#include "RAK3172.h"
#include "logger.h"
#include "homeassistant_mqtt.h"

TaskHandle_t xRAK3172ReceiveTaskHandle;

void vMessageProcessTask(void *pvParameters);

static const char *TAG = "[Receiver]";

static void vHomeAssistantMqttTask(void *pvParameters)
{
    (void)pvParameters;

    EventBits_t bits = xEventGroupWaitBits(
        network_event_group,
        WIFI_STA_IPV4_OBTAINED_BIT,
        pdFALSE,
        pdTRUE,
        portMAX_DELAY);

    if ((bits & WIFI_STA_IPV4_OBTAINED_BIT) != 0) {
        esp_err_t err = homeassistant_mqtt_start();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Could not start Home Assistant MQTT client: %s", esp_err_to_name(err));
        }
    }

    vTaskDelete(NULL);
}

// App entrypoint
void app_main(void)
{
    vLoggerTask_Init(10);

#if CONFIG_HA_MQTT_ENABLE
    wifi_init();
    xTaskCreate(vHomeAssistantMqttTask, "HA_MQTT", 4096, NULL, 5, NULL);
#endif

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
            homeassistant_mqtt_publish_measurements(msg.level, msg.rssi, msg.snr);
        }
    }
}
