#include "soft_uart.h"
static softuart_tx_t softuart_tx;
static softuart_fifo_t softuart_tx_fifo;
static softuart_rx_t softuart_rx;
static softuart_rx_fifo_t softuart_rx_fifo;


/**/
static uint8_t fifo_is_empty(void)
{
    return (softuart_tx_fifo.head == softuart_tx_fifo.tail);
}

static uint8_t fifo_is_full(void)
{
    return ((softuart_tx_fifo.head + 1) % SOFTUART_TX_BUF_SIZE
             == softuart_tx_fifo.tail);
}

static void fifo_push(uint8_t data)
{
    if (fifo_is_full())
        return;

    softuart_tx_fifo.buf[softuart_tx_fifo.head] = data;
    softuart_tx_fifo.head =
        (softuart_tx_fifo.head + 1) % SOFTUART_TX_BUF_SIZE;
}

static uint8_t fifo_pop(uint8_t *data)
{
    if (fifo_is_empty())
        return 0;

    *data = softuart_tx_fifo.buf[softuart_tx_fifo.tail];
    softuart_tx_fifo.tail =
        (softuart_tx_fifo.tail + 1) % SOFTUART_TX_BUF_SIZE;

    return 1;
}



void SoftUart_TX_Tick(void)
{
    if (!softuart_tx.busy)
        return;

    switch (softuart_tx.state)
    {
        case SOFTUART_START:
            /* 起始位：拉低 */
            SOFTUART_TX_LOW();
            softuart_tx.state = SOFTUART_DATA;
            softuart_tx.bit_index = 0;
            break;

        case SOFTUART_DATA:
            /* 发送数据位，低位先行 */
            if (softuart_tx.tx_byte & (1 << softuart_tx.bit_index))
                SOFTUART_TX_HIGH();
            else
                SOFTUART_TX_LOW();

            softuart_tx.bit_index++;

            if (softuart_tx.bit_index >= 8)
            {
                softuart_tx.state = SOFTUART_STOP;
            }
            break;
				case SOFTUART_STOP:
					/* 停止位：拉高 */
					SOFTUART_TX_HIGH();

					/* 当前字节完成 */
					softuart_tx.busy = 0;
					softuart_tx.state = SOFTUART_IDLE;

					/* FIFO 中还有数据 → 立刻续发 */
					if (fifo_pop(&softuart_tx.tx_byte))
					{
							softuart_tx.state = SOFTUART_START;
							softuart_tx.busy  = 1;
							/* 清零计数器，启动定时器 */
							SOFTUART_TIM_TX_START();
					}
					else
					{
							/* 无数据，关闭定时器 */
							SOFTUART_TIM_TX_STOP();
					}
					break;
        default:
            break;
    }
}
void SoftUart_SendByte(uint8_t byte)
{
    /* 忙则不发送 */
    /* 如果当前正在发送，则入 FIFO */
    if (softuart_tx.busy)
    {
        fifo_push(byte);
        return;
    }

    softuart_tx.tx_byte = byte;
    softuart_tx.state   = SOFTUART_START;
    softuart_tx.busy    = 1;

    /* 空闲时 TX 必须为高 */
    SOFTUART_TX_HIGH();

    /* 清零计数器，启动定时器 */
    SOFTUART_TIM_TX_START();
}

void SoftUart_SendString(const char *str)
{
    if (str == NULL)
        return;

    while (*str)
    {
        SoftUart_SendByte((uint8_t)*str++);
    }
}

void SoftUart_SendBuffer(const uint8_t *buf, uint16_t len)
{
    uint16_t i;

    if (buf == NULL || len == 0)
        return;

    for (i = 0; i < len; i++)
    {
        SoftUart_SendByte(buf[i]);
    }
}


void SOFT_RX_EXIT(void)
{
/* 忙则忽略 */
        if (softuart_rx.busy)
            return;

        /* 确认仍然是低电平（防抖） */
        if (SOFTUART_RX_READ() == Bit_RESET)
        {
            softuart_rx.busy  = 1;
            softuart_rx.state = SOFTUART_RX_START;
            softuart_rx.bit_index = 0;
            softuart_rx.rx_byte = 0;

            /* 定时器设置为 0.5 bit */
            SOFTUART_TIM_HALF_BIT();
        }
}
uint8_t SoftUart_RxFifoIsEmpty(void)
{
    return (softuart_rx_fifo.head == softuart_rx_fifo.tail);
}
uint8_t SoftUart_RxFifoIsFull(void)
{
    return ((softuart_rx_fifo.head + 1) % SOFTUART_RX_FIFO_SIZE
             == softuart_rx_fifo.tail);
}
static void SoftUart_RxFifoPush(uint8_t byte)
{
    uint16_t next;

    next = (softuart_rx_fifo.head + 1) % SOFTUART_RX_FIFO_SIZE;

    if (next == softuart_rx_fifo.tail)
    {
        /* FIFO 满：直接丢弃 */
        return;
    }

    softuart_rx_fifo.buf[softuart_rx_fifo.head] = byte;
    softuart_rx_fifo.head = next;
}
uint8_t SoftUart_ReadByte(uint8_t *byte)
{
    if (SoftUart_RxFifoIsEmpty())
        return 0;

    *byte = softuart_rx_fifo.buf[softuart_rx_fifo.tail];
    softuart_rx_fifo.tail =
        (softuart_rx_fifo.tail + 1) % SOFTUART_RX_FIFO_SIZE;

    return 1;
}

void SoftUart_RX_Tick(void)
{
    if (!softuart_rx.busy)
        return;

    switch (softuart_rx.state)
    {
        case SOFTUART_RX_START:
            SOFTUART_TIM_RX_START();

            softuart_rx.state = SOFTUART_RX_DATA;
            break;

        case SOFTUART_RX_DATA:
            if (SOFTUART_RX_READ())
                softuart_rx.rx_byte |= (1 << softuart_rx.bit_index);

            softuart_rx.bit_index++;

            if (softuart_rx.bit_index >= 8)
                softuart_rx.state = SOFTUART_RX_STOP;
            break;

        case SOFTUART_RX_STOP:
            /* 停止位应为高 */
            if (SOFTUART_RX_READ() == Bit_SET)
            {
								SoftUart_RxFifoPush(softuart_rx.rx_byte);
            }

            /* 接收结束 */
            softuart_rx.busy  = 0;
            softuart_rx.state = SOFTUART_RX_IDLE;

            SOFTUART_TIM_RX_STOP();
            break;

        default:
            break;
    }
}
