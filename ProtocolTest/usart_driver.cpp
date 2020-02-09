/*
 * usart_driver.cpp
 *
 * Created: 2/5/2020 10:16:52 AM
 *  Author: andrew.shkolik
 */ 

#include "usart_driver.h"
uint8_t pulsesBuff[USART_TX_PACKET_SIZE];

uint8_t * UsartTxBuffer = pulsesBuff;//(uint8_t*)pulses1MHz;  //[USART_TX_PACKET_SIZE] bytes used

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

#ifdef DSM_SERIAL
void UsartSet125000BAUDS() //DSM Serial protocol
{
#undef BAUD
#define BAUD 125000
//#include <util/setbaud.h>
  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART));
#else
   UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART));
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