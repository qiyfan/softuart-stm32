#include "soft_uart_port.h"


void SOFTUART_TX_HIGH(void){
		GPIO_SetBits(GPIOA, GPIO_Pin_9);    // PA9 = 1
}

void SOFTUART_TX_LOW(void){
		GPIO_ResetBits(GPIOA,GPIO_Pin_9);
}

void SOFTUART_TIM_RX_STOP(void){
			/* 立刻关闭定时器 */
     TIM_Cmd(TIM2, DISABLE);
}

void SOFTUART_TIM_RX_START(void){
	/* 清零计数器，启动定时器 */
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);
}

void SOFTUART_TIM_HALF_BIT(void)
{
						TIM_SetAutoreload(TIM3, (UART_BIT_US / 2) - 1);
            TIM_SetCounter(TIM3, 0);
            TIM_Cmd(TIM3, ENABLE);
}
void SOFTUART_TIM_TX_STOP(void){
			/* 立刻关闭定时器 */
     TIM_Cmd(TIM2, DISABLE);
}

void SOFTUART_TIM_TX_START(void){
	/* 清零计数器，启动定时器 */
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);
}
