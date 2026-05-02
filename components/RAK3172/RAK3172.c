#include "driver/gpio.h"
#include "driver/uart.h"
#include "string.h"
#include "esp_log.h"
#include "RAK3172.h"

#define LINE_BUFFER_SIZE 128

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
}

void RAK3172_SendCommand(const char* command)
{
    uart_write_bytes(UART_RAK3172_NUM, command, strlen(command));
    uart_write_bytes(UART_RAK3172_NUM, "\r\n", 2);
}

// void uartRAK3172_receiveTask(void *pvParameters)
// {
//     static const char *TAG_RAK3172 = "[RAK3172] ";

//     uint8_t data[BUF_SIZE + 1]; // +1 for null-termination
    
//     while (1) 
//     {
//         int len = uart_read_bytes(UART_RAK3172_NUM, data, BUF_SIZE, pdMS_TO_TICKS(20));
//         if (len > 0) 
//         {
//             data[len] = '\0';   // Null-terminate the received data

//             // Print any Response of RAK3172
//             ESP_LOGI(TAG_RAK3172, "%s", data);
//         }   
//     }
// }

void vRAK3172ReceiveTask(void *pvParameters)
{
    static const char *TAG_RAK3172 = "[RAK3172] ";

    uint8_t rx[BUF_SIZE];
    char line[LINE_BUFFER_SIZE];
    uint16_t idx = 0;

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

                if (RAK3172_DecoderRx(line, &msg) == 0)
                {
                    ESP_LOGI(TAG_RAK3172,"ID=%u RSSI=%d SNR=%d Level=%.2f litros", msg.device_id, msg.rssi, msg.snr, msg.level);
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
                idx = 0;
            }
        }
    }
    }

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

int RAK3172_DecoderRx(const char *s, RAK3172_Message_t *msg)
{
    if (!s || !msg) return -1;

    const char *prefix = "+EVT:RXP2P:";
    for (int i = 0; prefix[i]; i++) {
        if (s[i] != prefix[i]) return -1;
    }

    const char *p = s + 12;

    // RSSI
    msg->rssi = parseSignedInt8(&p);
    if (*p++ != ':') return -1;

    // SNR
    msg->snr = parseSignedInt8(&p);
    if (*p++ != ':') return -1;

    // device_id (2 hex chars = 1 byte)
    int hi = hexCharToNibble(*p++);
    int lo = hexCharToNibble(*p++);

    if (hi < 0 || lo < 0) return -1;

    msg->device_id = (uint8_t)((hi << 4) | lo);

    // level (8 hex chars = 4 bytes)
    uint8_t float_bytes[4];

    for (int i = 0; i < 4; i++)
    {
        hi = hexCharToNibble(*p++);
        lo = hexCharToNibble(*p++);

        if (hi < 0 || lo < 0) return -1;

        float_bytes[i] = (uint8_t)((hi << 4) | lo);
    }

    memcpy(&msg->level, float_bytes, sizeof(float));

    // total payload bytes parseados
    msg->length = 5;

    return 0;
}