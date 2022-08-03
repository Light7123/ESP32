
#ifndef UART_H_
#define UART_H_


#include "main.h"
#include "driver/uart.h"

#define UART1_TXD  (GPIO_NUM_1)
#define UART1_RXD  (GPIO_NUM_3)
#define UART1_RTS  (UART_PIN_NO_CHANGE)
#define UART1_CTS  (UART_PIN_NO_CHANGE)

#define UART2_TXD  (GPIO_NUM_17)
#define UART2_RXD  (GPIO_NUM_16)
#define UART2_RTS  (UART_PIN_NO_CHANGE)
#define UART2_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

void USART1_init(void);
void USART2_init(void);
void transmitUART_task(void *pvParameters);
void count_task(void *pvParameters);

#endif /* UART_H_ */