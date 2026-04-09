#ifndef RAK3172_H
#define RAK3172_H

#define UART_RAK3172_NUM   UART_NUM_1
#define RAK3172_TXD_PIN    GPIO_NUM_4
#define RAK3172_RXD_PIN    GPIO_NUM_5

// #define UART_RAK3172_NUM UART_NUM_1
// #define RAK3172_TXD_PIN (GPIO_NUM_0)
// #define RAK3172_RXD_PIN (GPIO_NUM_1)

#define BUF_SIZE 1024

typedef struct {
    uint8_t length;
    uint8_t rssi;
    uint8_t snr;
    uint8_t device_id;
    char payload_hex[64];
} RAK3172_Message;

void init_RAK3172(void);
void RAK3172_sendCommand(const char* command);
void uartRAK3172_receiveTask(void *pvParameters);

#endif