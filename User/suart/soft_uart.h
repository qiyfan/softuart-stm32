#ifndef _SOFT_UART_H_
#define _SOFT_UART_H_
#include "stdint.h"
#include "stdio.h"
#include "soft_uart_port.h"




typedef enum
{
    SOFTUART_IDLE = 0,
    SOFTUART_START,
    SOFTUART_DATA,
    SOFTUART_STOP,
} softuart_state_t;

typedef struct
{
    softuart_state_t state;
    uint8_t  tx_byte;
    uint8_t  bit_index;
    uint8_t  busy;
} softuart_tx_t;

#define SOFTUART_TX_BUF_SIZE   128

typedef struct
{
    uint8_t buf[SOFTUART_TX_BUF_SIZE];
    uint16_t head;
    uint16_t tail;
} softuart_fifo_t;

typedef enum
{
    SOFTUART_RX_IDLE = 0,
    SOFTUART_RX_START,
    SOFTUART_RX_DATA,
    SOFTUART_RX_STOP,
} softuart_rx_state_t;

typedef struct
{
    softuart_rx_state_t state;
    uint8_t rx_byte;
    uint8_t bit_index;
    uint8_t busy;
} softuart_rx_t;


#define SOFTUART_RX_FIFO_SIZE   64

typedef struct
{
    uint8_t  buf[SOFTUART_RX_FIFO_SIZE];
    uint16_t head;
    uint16_t tail;
} softuart_rx_fifo_t;





void SoftUart_TX_Tick(void);
void SoftUart_SendByte(uint8_t byte);
void SoftUart_SendString(const char *str);
void SoftUart_SendBuffer(const uint8_t *buf, uint16_t len);
uint8_t SoftUart_ReadByte(uint8_t *byte);
#endif
