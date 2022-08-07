/*
 * usart_ring.c
 *
 *  Created on: 1 нояб. 2020 г.
 *      Author: moroz
 */

#include "usart_ring.h"

//---------------------------------------------------DEBUG USART---------------------------------------------------
volatile dbg_rx_buffer_index_t dbg_rx_buffer_head = 0;
volatile dbg_rx_buffer_index_t dbg_rx_buffer_tail = 0;
uint8_t dbg_rx_buffer[DBG_RX_BUFFER_SIZE] = {0,};

int16_t dbg_available(void)
{
	return ((uint16_t)(DBG_RX_BUFFER_SIZE + dbg_rx_buffer_head - dbg_rx_buffer_tail)) % DBG_RX_BUFFER_SIZE;
}

int16_t dbg_read(void)
{
	if(dbg_rx_buffer_head == dbg_rx_buffer_tail)
	{
		return -1;
	}
	else
	{
		uint8_t c = dbg_rx_buffer[dbg_rx_buffer_tail];
		dbg_rx_buffer_tail = (dbg_rx_buffer_index_t)(dbg_rx_buffer_tail + 1) % DBG_RX_BUFFER_SIZE;
		return c;
	}
}
//-----------------------------------------------------------------------------------------------------------------
void EN_Interrupt(void)
{
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	//__HAL_SPI_ENABLE_IT(&hspi2, SPI_IT_RXNE);
}
