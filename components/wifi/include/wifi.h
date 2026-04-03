#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

/**
 * @brief Event group bits for WiFi events
 */
#define WIFI_STA_CONNECTED_BIT      BIT0
#define WIFI_STA_IPV4_OBTAINED_BIT  BIT1
#define WIFI_STA_IPV6_OBTAINED_BIT  BIT2

// Notification values
#define WIFI_NOTIFY_CONNECTED       0x01
#define WIFI_NOTIFY_DISCONNECTED    0x02
#define WIFI_NOTIFY_GOT_IP          0x03

// Event group handle for WiFi events. Set by wifi_sta_init() if event_group is NULL.
extern EventGroupHandle_t network_event_group;

/**
 * @brief Configuration WiFi
 * 
 * This function initializes the WiFi driver and sets up event handlers for WiFi
 * and IP events. It does not start the WiFi driver or connect to a network -
 * call wifi_sta_init() to do that.
 * 
 * Important! you must call esp_netif_init() and esp_event_loop_create_default()
 * before calling this function.
 * 
 * @return
 *  - ESP_OK on success
 *  - Other errors on failure. See esp_err.h for error codes.
 */
void wifi_init(void);

/* *
 * @brief Register a task to receive WiFi event notifications
 * 
 * This function allows you to register a FreeRTOS task to receive notifications
 * about WiFi events (connected, disconnected, got IP). The task will receive
 * notifications via the task notification mechanism (ulTaskNotifyTake, xTaskNotifyWait).
 * 
 * @param[in] task_handle Handle of the task to register for notifications.
 */
// void wifi_register_task(TaskHandle_t task_handle);

/**
 * @brief Initialize WiFi in station (STA) mode.
 * 
 * Set up the WiFi interface and connect to a WiFi network. You can use the
 * event group to wait for a connection and IP address assignment. 
 * 
 * Important! you must call esp_netif_init() and esp_event_loop_create_default()
 * before calling this function.
 * 
 * @param[in] event_group Event group handle for WiFi and IP events. Pass NULL
 *                        to use the existing event group.
 * 
 * @return
 *  - ESP_OK on success
 *  - Other errors on failure. See esp_err.h for error codes.
 */
esp_err_t wifi_sta_init(EventGroupHandle_t event_group);

/**
 * @brief Disable WiFi
 * 
 * @return
 *  - ESP_OK on success
 *  - Other errors on failure. See esp_err.h for error codes.
 */
esp_err_t wifi_sta_stop(void);

/**
 * @brief Attempt to reconnect WiFi
 * 
 * @return
 *  - ESP_OK on success
 *  - Other errors on failure. See esp_err.h for error codes.
 */
esp_err_t wifi_sta_reconnect(void);

#endif // WIFI_H