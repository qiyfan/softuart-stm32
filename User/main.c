/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   串口接发测试，串口接收到数据后马上回传。
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "soft_uart.h"
#include "soft_uart_port.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "soft_uart_drive_init.h"
#include "soft_uart.h"

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
	#if 0
int main(void)
{	
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  Debug_USART_Config();
	
	/* 发送一个字符串 */
	Usart_SendString( DEBUG_USART,"这是一个串口中断接收回显实验\n");
	printf("这是一个串口中断接收回显实验\n");
	
  while(1)
	{	
		
	}	
}
#endif
volatile uint32_t TimingDelay = 0;
void SysTick_Init(void)
{
    /* 配置 SysTick 为 1ms 中断 */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        /* 配置失败，死循环 */
        while (1);
    }
}
void SysTick_Handler(void)
{
    if (TimingDelay > 0)
    {
        TimingDelay--;
    }
}
void delay_ms(uint32_t ms)
{
    TimingDelay = ms;
    while (TimingDelay != 0);
}
int main(void)
{		
		uint8_t ch;
		SysTick_Init();
	
    DEBUG_USART_GPIO_Config();
		TIM2_Init(SOFT_UART_baudrate);
		TIM3_Init(SOFT_UART_baudrate);
	
		delay_ms(1000);
	while(1){
		#if 0
		SoftUart_SendByte(0x78);
		delay_ms(1000);
		SoftUart_SendString("Hello Soft UART\r\n");
		delay_ms(1000);
		uint8_t data[] = {0x55, 0xAA, 0x01, 0x02};
		SoftUart_SendBuffer(data, sizeof(data));
		delay_ms(1000);
		#endif
		
		#if 1
		if (SoftUart_ReadByte(&ch))
		{
				/* 收到一个字节 */
				SoftUart_SendByte(ch);
		}
		delay_ms(100);
		#endif
	}
   
}

/*********************************************END OF FILE**********************/

