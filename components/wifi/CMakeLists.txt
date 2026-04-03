set(srcs)
set(include_dirs "include")

if(CONFIG_WIFI_STA_CONNECT)
    list(APPEND srcs
        "wifi.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_REQUIRES driver wifi esp_event esp_wifi esp_netif nvs_flash freertos)
