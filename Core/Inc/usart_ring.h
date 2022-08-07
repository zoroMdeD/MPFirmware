/*
 * usart_ring.h
 *
 *  Created on: 6 авг. 2022 г.
 *      Author: moroz
 */

#ifndef INC_USART_RING_H_
#define INC_USART_RING_H_

#include "main.h"

void EN_Interrupt(void);

//---------------------------------------------------DEBUG USART---------------------------------------------------
#define DBG_RX_BUFFER_SIZE 1030	//default: 64

#if (DBG_RX_BUFFER_SIZE > 256)
	typedef uint16_t dbg_rx_buffer_index_t;
#else
	typedef uint8_t dbg_rx_buffer_index_t;
#endif

int16_t dbg_available(void);
int16_t dbg_read(void);
//-----------------------------------------------------------------------------------------------------------------

#endif /* INC_USART_RING_H_ */
