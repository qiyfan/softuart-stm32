#ifndef _SOFT_UART_PORT_
#define _SOFT_UART_PORT_

#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"


#define SOFT_UART_baudrate 115200
#define UART_BIT_US  1000000 / SOFT_UART_baudrate
#define SOFTUART_RX_READ() \
    GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10)



void SOFTUART_TX_HIGH(void);
void SOFTUART_TX_LOW(void);
void SOFTUART_TIM_RX_STOP(void);
void SOFTUART_TIM_RX_START(void);
void SOFTUART_TIM_HALF_BIT(void);
void SOFTUART_TIM_TX_STOP(void);
void SOFTUART_TIM_TX_START(void);
void SOFTUART_TX_TOGGLE(void);

#endif
