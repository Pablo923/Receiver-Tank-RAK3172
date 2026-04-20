#include "driver/gpio.h"
#include "driver/uart.h"
#include "string.h"
#include "esp_log.h"
#include "RAK3172.h"


void init_RAK3172(void) {
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

void RAK3172_sendCommand(const char* command)
{
    uart_write_bytes(UART_RAK3172_NUM, command, strlen(command));
    uart_write_bytes(UART_RAK3172_NUM, "\r\n", 2);
}

void uartRAK3172_receiveTask(void *pvParameters)
{
    static const char *TAG_RAK3172 = "[RAK3172] ";

    uint8_t data[BUF_SIZE + 1];  // +1 para null terminator
    
    while (1) 
    {
        int len = uart_read_bytes(UART_RAK3172_NUM, data, BUF_SIZE, pdMS_TO_TICKS(20));
        if (len > 0) 
        {
            data[len] = '\0';   // Null-terminate the received data

            // Print any Response of RAK3172
            ESP_LOGI(TAG_RAK3172, "%s", data);
        }   
    }
}