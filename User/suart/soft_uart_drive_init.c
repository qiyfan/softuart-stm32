#include "soft_uart_drive_init.h"

extern void SoftUart_TX_Tick(void);
extern void SOFT_RX_EXIT(void);
extern void SoftUart_RX_Tick(void);

void SoftUart_RX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PA10 -> EXTI10 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,
                          EXTI_PinSource10);

    EXTI_InitStruct.EXTI_Line    = EXTI_Line10;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // 起始位
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}
void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line10) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line10);
				
				SOFT_RX_EXIT();
        
    }
}
void SOFTUART_TX_TOGGLE(void)
{
    if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_3))
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_3);   // 当前为高，拉低
    }
    else
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_3);     // 当前为低，拉高
    }
}

void DEBUG_USART_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 1. 使能 GPIOA 时钟 */
    RCC_AHB1PeriphClockCmd(DEBUG_USART_TX_GPIO_CLK, ENABLE);

    /* 2. 配置 PA9 为普通 GPIO 输出 */
    GPIO_InitStructure.GPIO_Pin   = DEBUG_USART_TX_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;        // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        // 推挽输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;         // 上拉（可按需改为 NOPULL）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	
		//调试用
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	    /* 3. 配置 PA10 为普通 GPIO 输入 */
			SoftUart_RX_GPIO_Init();
}

uint32_t UART_BaudrateToPeriodUs(uint32_t baudrate)
{
    return 1000000 / baudrate;   // 单位：us
}

void TIM2_Init(uint32_t baudrate)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef         NVIC_InitStruct;

    uint32_t period_us;

    /* 使能 TIM2 时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* 1MHz 计数频率（1us） */
    period_us = UART_BaudrateToPeriodUs(baudrate);

    TIM_TimeBaseInitStruct.TIM_Prescaler     = 83;                  // 84MHz / (83+1) = 1MHz
    TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period        = period_us - 1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    /* 使能更新中断 */
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    /* NVIC 配置 */
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* 启动定时器 */
    //TIM_Cmd(TIM2, ENABLE);
}
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        /* 每个 bit 时间进一次中断 */
        SoftUart_TX_Tick();
			
    }
}
void TIM3_Init(uint32_t baudrate)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef         NVIC_InitStruct;

    uint32_t period_us;

    /* 使能 TIM2 时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* 1MHz 计数频率（1us） */
    period_us = UART_BaudrateToPeriodUs(baudrate);

    TIM_TimeBaseInitStruct.TIM_Prescaler     = 83;                  // 84MHz / (83+1) = 1MHz
    TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period        = period_us - 1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

    /* 使能更新中断 */
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /* NVIC 配置 */
    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* 启动定时器 */
    //TIM_Cmd(TIM3, ENABLE);
}
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

        /* 每个 bit 时间进一次中断状态机 */
        SoftUart_RX_Tick();
				SOFTUART_TX_TOGGLE();
    }
}
