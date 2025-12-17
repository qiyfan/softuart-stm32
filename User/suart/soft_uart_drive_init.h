#ifndef _SOFT_UART_DRIVE_INIT_H_
#define _SOFT_UART_DRIVE_INIT_H_

#include "stm32f4xx.h"
#include <stdio.h>


#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_TX_PIN                      GPIO_Pin_9

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_RX_PIN                      GPIO_Pin_10


void DEBUG_USART_GPIO_Config(void);
void TIM2_Init(uint32_t baudrate);
void TIM3_Init(uint32_t baudrate);

#endif
