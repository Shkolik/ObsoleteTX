/*
 * Copyright (C) ObsoleteTX
 *
 * Based on code named
 *   th9x - https://github.com/thus1/th9x
 *   er9x - https://github.com/MikeBland/mbtx
 *   OpenTx - https://github.com/opentx/opentx
 *   OpenAVRc - https://github.com/Ingwie/OpenAVRc_Dev
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */ 

#include "usart_driver.h"
uint8_t pulsesBuff[USART_TX_PACKET_SIZE];

uint8_t * UsartTxBuffer = pulsesBuff;//[USART_TX_PACKET_SIZE] bytes used

uint8_t UsartTxBufferCount = 0;

void UsartEnableTx()
{
  UCSRB_N(TLM_USART) |= (1 << TXEN_N(TLM_USART)); // enable TX
}

void UsartDisableTx()
{
  UCSRB_N(TLM_USART) &= ~(1 << UDRIE_N(TLM_USART));// disable Interrupt
  UCSRB_N(TLM_USART) &= ~(1 << TXEN_N(TLM_USART)); // disable TX
}

#ifdef TELEMETRY
void UsartEnableRx()
{
	UCSRB_N(TLM_USART) |= (1 << RXEN_N(TLM_USART));  // enable RX
	UCSRB_N(TLM_USART) |= (1 << RXCIE_N(TLM_USART)); // enable Interrupt
	while (UCSRA_N(TLM_USART) & (1 << RXC_N(TLM_USART))) UDR_N(TLM_USART); // Flush RX buffer.
}

void UsartDisableRx()
{
  UCSRB_N(TLM_USART) &= ~(1 << RXCIE_N(TLM_USART)); // disable Interrupt
  UCSRB_N(TLM_USART) &= ~(1 << RXEN_N(TLM_USART));  // disable RX
}
#endif

void UsartSet8N1()
{
  UCSRB_N(TLM_USART) = (0 << RXCIE_N(TLM_USART)) | (0 << TXCIE_N(TLM_USART)) | (0 << UDRIE_N(TLM_USART)) | (0 << RXEN_N(TLM_USART)) | (0 << TXEN_N(TLM_USART)) | (0 << UCSZ2_N(TLM_USART));
  UCSRC_N(TLM_USART) = (1 << UCSZ1_N(TLM_USART)) | (1 << UCSZ0_N(TLM_USART)); // Set 1 stop bit, No parity bit.
}

void UsartSet8E2()
{
  UCSRB_N(TLM_USART) = (0 << RXCIE_N(TLM_USART)) | (0 << TXCIE_N(TLM_USART)) | (0 << UDRIE_N(TLM_USART)) | (0 << RXEN_N(TLM_USART)) | (0 << TXEN_N(TLM_USART)) | (0 << UCSZ2_N(TLM_USART));
  UCSRC_N(TLM_USART) = (1 << UPM01) | (1 << USBS0)| (1 << UCSZ1_N(TLM_USART)) | (1 << UCSZ0_N(TLM_USART)); // set 2 stop bits, even parity BIT
}

#ifdef DSM
void UsartSet125000BAUDS() //DSM Serial protocol
{
#undef BAUD
#define BAUD 125000
#if F_CPU < 16000000UL
	// it's a hack for 8Mhz atmega128 in 1x mode
	// TODO: Make more correct calculations in future
	uint16_t baud_setting = 3;//7;
	UBRRH_N(TLM_USART) = baud_setting >> 8;
	UBRRL_N(TLM_USART) = baud_setting;
#else
	#include <util/setbaud.h>
	UBRRH_N(TLM_USART) = UBRRH_VALUE;
	UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if USE_2X
	(TLM_USART) |= (1 << U2X_N(TLM_USART));
#else
	UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART));
#endif
#endif
}
#endif

#ifdef MULTI
void UsartSet100000BAUDS() //Multiprotocol Serial
{
#undef BAUD
#define BAUD 100000

#if F_CPU < 16000000UL
	// it's a hack for 8Mhz atmega128 in 1x mode
	// TODO: Make more correct calculations in future
	uint16_t baud_setting = 4;//8;
	UBRRH_N(TLM_USART) = baud_setting >> 8;
	UBRRL_N(TLM_USART) = baud_setting;
#else
#include <util/setbaud.h>
	UBRRH_N(TLM_USART) = UBRRH_VALUE;
	UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if USE_2X
	UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART));
#else
	UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART));
#endif
#endif
}
#endif

void UsartTransmitBuffer()
{
	UCSRB_N(TLM_USART) |= (1 << UDRIE_N(TLM_USART)); // enable Data Register Empty Interrupt
}

// USART0 Transmit Data Register Empty ISR (UDR was loaded in Shift Register)
ISR(USART_UDRE_vect_N(TLM_USART))
{
  if (UsartTxBufferCount > 0) {
    UDR_N(TLM_USART) = UsartTxBuffer[--UsartTxBufferCount];
  }
  else {
    UCSRB_N(TLM_USART) &= ~(1 << UDRIE_N(TLM_USART)); // Disable UDRE interrupt.
  }
}

NOINLINE void parseTelemFrskyByte(uint8_t data)
{
	debug(data);
}