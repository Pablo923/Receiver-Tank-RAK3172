#ifndef RAK3172_H
#define RAK3172_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"

#define UART_RAK3172_NUM   UART_NUM_1
#define RAK3172_TXD_PIN    GPIO_NUM_4
#define RAK3172_RXD_PIN    GPIO_NUM_5

#define BUF_SIZE 1024
#define RAK3172_P2P_CONFIG "915000000:7:125:0:10:14"

/*
 * LoRa payload sent by ESP32_Water_Tank_Level (9 bytes, little-endian):
 *   [0]    device ID (uint8)
 *   [1..4] water level, liters (float32)
 *   [5..6] temperature, hundredths of degC (int16), 0x8000 = no data
 *   [7..8] humidity, hundredths of %RH (uint16), 0xFFFF = no data
 * The legacy 4-byte payload (water level only) is still accepted.
 */
#define RAK3172_PAYLOAD_LEGACY_SIZE   4
#define RAK3172_PAYLOAD_SIZE          9
#define RAK3172_TEMPERATURE_NO_DATA   INT16_MIN   // 0x8000
#define RAK3172_HUMIDITY_NO_DATA      UINT16_MAX  // 0xFFFF

typedef struct {
    uint8_t  length;            // Payload length in bytes (4 or 9)
    int8_t   rssi;
    int8_t   snr;
    uint8_t  device_id;         // 0 for legacy payloads
    float    level;             // Liters
    int16_t  temperature_c100;  // RAK3172_TEMPERATURE_NO_DATA when unavailable
    uint16_t humidity_rh100;    // RAK3172_HUMIDITY_NO_DATA when unavailable
} RAK3172_Message_t;

/* Funtions */
void RAK3172_Init(void);
void RAK3172_StartReceiver(void);
void RAK3172_SendCommand(const char* command);
int  RAK3172_DecoderRx(const char *s, RAK3172_Message_t *msg);

/* Tasks */
void vRAK3172ReceiveTask(void *pvParameters);

/*
 * Dequeue a message from the RAK3172 message queue to use outsite of this file.
 * 
 * @param pxMessage Pointer to a RAK3172_Message_t structure where the dequeued message will be stored.
 * @param xTicksToWait Maximum time to wait for a message to be available. Use portMAX_DELAY for indefinite wait.
 * @return pdPASS if a message was successfully dequeued, otherwise pdFAIL.
 */
BaseType_t RAK3172_DequeueMessage(
    RAK3172_Message_t *pxMessage,
    TickType_t xTicksToWait
);

#endif
