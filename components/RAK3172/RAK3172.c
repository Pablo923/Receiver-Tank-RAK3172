#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdbool.h>
#include <stdio.h>
#include "string.h"
#include "RAK3172.h"

#define LINE_BUFFER_SIZE 128

static const char *TAG_RAK3172 = "[RAK3172]";
static QueueHandle_t xMessageQueue = NULL;

static inline int8_t parseSignedInt8(const char **p);
static inline int hexCharToNibble(char c);
static bool parseRxMetadata(const char *line, int8_t *rssi, int8_t *snr);
static int decodePayload(const char *hex, RAK3172_Message_t *msg, int8_t rssi, int8_t snr);

/* PUBLIC FUNCTIONS */

void RAK3172_Init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    // Init RAK3172 Communication Port
    uart_driver_install(UART_RAK3172_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_RAK3172_NUM, &uart_config);
    uart_set_pin(UART_RAK3172_NUM, RAK3172_TXD_PIN, RAK3172_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Create queue for received messages
    xMessageQueue = xQueueCreate(10, sizeof(RAK3172_Message_t));
    configASSERT(xMessageQueue);
}

void RAK3172_StartReceiver(void)
{
    char command[48];

    // P2P mode and radio parameters must match the transmitter.
    RAK3172_SendCommand("AT+NWM=0");
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Stop RX before changing P2P parameters.
    RAK3172_SendCommand("AT+PRECV=0");
    vTaskDelay(pdMS_TO_TICKS(100));

    snprintf(command, sizeof(command), "AT+P2P=%s", RAK3172_P2P_CONFIG);
    RAK3172_SendCommand(command);
    vTaskDelay(pdMS_TO_TICKS(200));

    RAK3172_SendCommand("AT+PRECV=65534");
}

void RAK3172_SendCommand(const char* command)
{
    ESP_LOGI(TAG_RAK3172, "AT -> %s", command);
    uart_write_bytes(UART_RAK3172_NUM, command, strlen(command));
    uart_write_bytes(UART_RAK3172_NUM, "\r\n", 2);
}

int RAK3172_DecoderRx(const char *s, RAK3172_Message_t *msg)
{
    if (!s || !msg) return -1;

    const char *prefix = "+EVT:RXP2P:";
    if (strncmp(s, prefix, strlen(prefix)) != 0) return -1;
    const char *p = s + strlen(prefix);

    // RSSI
    if (*p != '-' && (*p < '0' || *p > '9')) return -1;
    msg->rssi = parseSignedInt8(&p);
    if (*p++ != ':') return -1;

    // SNR
    if (*p != '-' && (*p < '0' || *p > '9')) return -1;
    msg->snr = parseSignedInt8(&p);
    if (*p++ != ':') return -1;

    return decodePayload(p, msg, msg->rssi, msg->snr);
}

// Function to dequeue messages from the RAK3172 message from out
BaseType_t RAK3172_DequeueMessage(RAK3172_Message_t *pxMessage, TickType_t xTicksToWait)
{
    return xQueueReceive(xMessageQueue, pxMessage, xTicksToWait);
}

/* TASKS */

void vRAK3172ReceiveTask(void *pvParameters)
{
    uint8_t rx[BUF_SIZE];
    char line[LINE_BUFFER_SIZE];
    uint16_t idx = 0;
    bool have_rx_metadata = false;
    int8_t rx_rssi = 0;
    int8_t rx_snr = 0;

    RAK3172_Message_t msg;
    
    while (1) 
    {
        int len = uart_read_bytes(UART_RAK3172_NUM, rx, sizeof(rx), pdMS_TO_TICKS(20));
        
        for (int i = 0; i < len; i++)
        {
            char c = rx[i];

            // Ignore carriage return
            if (c == '\r') continue;
            // Newline indicates end of line
            if (c == '\n')
            {
                line[idx] = '\0';
                if (idx > 0) ESP_LOGI(TAG_RAK3172, "UART <- %s", line);

                if (parseRxMetadata(line, &rx_rssi, &rx_snr))
                {
                    have_rx_metadata = true;
                }
                else if (strncmp(line, "+EVT:", 5) == 0 &&
                         decodePayload(line + 5, &msg,
                                       have_rx_metadata ? rx_rssi : 0,
                                       have_rx_metadata ? rx_snr : 0) == 0)
                {
                    if (xQueueSend(xMessageQueue, &msg, 0) != pdPASS)
                    {
                        ESP_LOGW(TAG_RAK3172, "Message queue full; dropping packet");
                    }
                    have_rx_metadata = false;
                }
                else if (RAK3172_DecoderRx(line, &msg) == 0)
                {
                    if (xQueueSend(xMessageQueue, &msg, 0) != pdPASS)
                    {
                        ESP_LOGW(TAG_RAK3172, "Message queue full; dropping packet");
                    }
                }

                idx = 0;
                continue;
            }

            // Add character to line buffer if there's space
            if (idx < sizeof(line) - 1)
            {
                line[idx++] = c;
            }
            else
            {
                ESP_LOGW(TAG_RAK3172, "UART line too long; discarding it");
                idx = 0;
            }
        }
    }
    }

/* PRIVATE FUNCTIONS */

/*
 * Parses an integer from a string, handling optional leading '-' for negative numbers.
 * Advances the pointer to the position after the parsed integer.
 */
static inline int8_t parseSignedInt8(const char **p)
{
    int sign = 1;
    int val = 0;

    if (**p == '-') {
        sign = -1;
        (*p)++;
    }

    while (**p >= '0' && **p <= '9') {
        val = val * 10 + (**p - '0');
        (*p)++;
    }

    return (int8_t)(val * sign);
}

/*
 * Converts a hexadecimal character to its corresponding 4-bit value (nibble).
 * Returns -1 if the character is not a valid hexadecimal digit.
*/
static inline int hexCharToNibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

static bool parseRxMetadata(const char *line, int8_t *rssi, int8_t *snr)
{
    const char *prefix = "+EVT:RXP2P, RSSI ";
    if (strncmp(line, prefix, strlen(prefix)) != 0) return false;

    const char *p = line + strlen(prefix);
    if (*p != '-' && (*p < '0' || *p > '9')) return false;
    *rssi = parseSignedInt8(&p);

    const char *snr_prefix = ", SNR ";
    if (strncmp(p, snr_prefix, strlen(snr_prefix)) != 0) return false;
    p += strlen(snr_prefix);

    if (*p != '-' && (*p < '0' || *p > '9')) return false;
    *snr = parseSignedInt8(&p);
    return *p == '\0';
}

static int decodePayload(const char *hex, RAK3172_Message_t *msg, int8_t rssi, int8_t snr)
{
    if (!hex || !msg || strlen(hex) != 8) return -1;

    uint8_t float_bytes[4];
    for (int i = 0; i < 4; i++)
    {
        int hi = hexCharToNibble(hex[i * 2]);
        int lo = hexCharToNibble(hex[i * 2 + 1]);
        if (hi < 0 || lo < 0) return -1;
        float_bytes[i] = (uint8_t)((hi << 4) | lo);
    }

    memcpy(&msg->level, float_bytes, sizeof(msg->level));
    msg->length = sizeof(float_bytes);
    msg->rssi = rssi;
    msg->snr = snr;
    return 0;
}
