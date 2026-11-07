#ifndef RAK3172_H
#define RAK3172_H

#define UART_RAK3172_NUM   UART_NUM_1
#define RAK3172_TXD_PIN    GPIO_NUM_4
#define RAK3172_RXD_PIN    GPIO_NUM_5

#define BUF_SIZE 1024
#define RAK3172_P2P_CONFIG "915000000:7:125:0:10:14"

typedef struct {
    uint8_t length;
    int8_t  rssi;
    int8_t  snr;
    float   level;
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
