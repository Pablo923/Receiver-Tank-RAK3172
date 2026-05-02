#ifndef RAK3172_H
#define RAK3172_H

#define UART_RAK3172_NUM   UART_NUM_1
#define RAK3172_TXD_PIN    GPIO_NUM_4
#define RAK3172_RXD_PIN    GPIO_NUM_5

#define BUF_SIZE 1024

typedef struct {
    uint8_t length;
    int8_t  rssi;
    int8_t  snr;
    uint8_t device_id;
    float   level;
} RAK3172_Message_t;

/* Funtions */
void RAK3172_Init(void);
void RAK3172_SendCommand(const char* command);
int  RAK3172_DecoderRx(const char *s, RAK3172_Message_t *msg);

/* Tasks*/
void vRAK3172ReceiveTask(void *pvParameters);

#endif