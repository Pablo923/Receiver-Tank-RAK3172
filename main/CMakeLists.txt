idf_component_register(SRCS "homeassistant_mqtt.c" "main.c"
                        INCLUDE_DIRS ""
                        PRIV_REQUIRES RAK3172 logger wifi mqtt esp_event esp_wifi esp_netif nvs_flash freertos)
