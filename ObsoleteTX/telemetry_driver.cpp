/*
 * telemetry_driver.cpp
 *
 * Created: 1/25/2020 8:53:38 PM
 *  Author: Andrew
 */ 

#include "ObsoleteTX.h"

uint8_t TelemetryRxBuffer[NUM_TELEM_RX_BUFFER][TELEM_RX_PACKET_SIZE];

uint8_t * Usart0TxBuffer = pulses2MHz.pbyte; // [USART_TX_PACKET_SIZE] bytes used

uint8_t Usart0TxBufferCount = 0;

void UsartEnableTx()
{
  UCSRB_N(TLM_USART) |= (1 << TXEN_N(TLM_USART)); // enable TX
}

void UsartEnableRx()
{
  UCSRB_N(TLM_USART) |= (1 << RXEN_N(TLM_USART));  // enable RX
  UCSRB_N(TLM_USART) |= (1 << RXCIE_N(TLM_USART)); // enable Interrupt
  while (UCSRA_N(TLM_USART) & (1 << RXC_N(TLM_USART))) UDR_N(TLM_USART); // Flush RX buffer.
}

void UsartDisableTx()
{
  UCSRB_N(TLM_USART) &= ~(1 << UDRIE_N(TLM_USART));// disable Interrupt
  UCSRB_N(TLM_USART) &= ~(1 << TXEN_N(TLM_USART)); // disable TX
}

void UsartDisableRx()
{
  UCSRB_N(TLM_USART) &= ~(1 << RXCIE_N(TLM_USART)); // disable Interrupt
  UCSRB_N(TLM_USART) &= ~(1 << RXEN_N(TLM_USART));  // disable RX
}

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

void UsartSet9600BAUDS() //Frsky "D" telemetry
{
#undef BAUD
#define BAUD 9600
#include <util/setbaud.h>
  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART));
#else
   UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART));
#endif
}

void UsartSet57600BAUDS() //Frsky S.port telemetry
{
#undef BAUD
#define BAUD 57600
#include <util/setbaud.h>
  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART));
#else
   UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART));
#endif
}

void UsartSet125000BAUDS() //DSM Serial protocol
{
#undef BAUD
#define BAUD 125000
#include <util/setbaud.h>
  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART));
#else
   UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART));
#endif
}

void UsartSet100000BAUDS() //Multiprotocol Serial
{
#undef BAUD
#define BAUD 100000
#include <util/setbaud.h>
  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART));
#else
   UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART));
#endif
}

void UsartTransmitBuffer()
{
  UCSRB_N(TLM_USART) |= (1 << UDRIE_N(TLM_USART)); // enable Data Register Empty Interrupt
}

#if defined(FRSKY) || defined(MULTI)

ISR(USART_RX_vect_N(TLM_USART))
{
  uint8_t stat;
  uint8_t data;

  UCSRB_N(TLM_USART) &= ~(1 << RXCIE_N(TLM_USART)); // disable Interrupt

  NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE)
  {

    stat = UCSRA_N(TLM_USART); // USART control and Status Register 0/1 A

    /*
                bit      7      6      5      4      3      2      1      0
                        RxC0  TxC0  UDRE0    FE0   DOR0   UPE0   U2X0  MPCM0

                RxC0:   Receive complete
                TXC0:   Transmit Complete
                UDRE0:  USART Data Register Empty
                FE0:    Frame Error
                DOR0:   Data OverRun
                UPE0:   USART Parity Error
                U2X0:   Double Tx Speed
                PCM0:   MultiProcessor Comms Mode
     */
    // rh = UCSRB_N(TLM_USART); //USART control and Status Register 0/1 B

    /*
              bit      7      6      5      4      3      2      1      0
                   RXCIE0 TxCIE0 UDRIE0  RXEN0  TXEN0 UCSZ02  RXB80  TXB80

              RxCIE0:   Receive Complete int enable
              TXCIE0:   Transmit Complete int enable
              UDRIE0:   USART Data Register Empty int enable
              RXEN0:    Rx Enable
              TXEN0:    Tx Enable
              UCSZ02:   Character Size bit 2
              RXB80:    Rx data bit 8
              TXB80:    Tx data bit 8
    */

    data = UDR_N(TLM_USART); // USART data register 0

    if (stat & ((1 << FE_N(TLM_USART)) | (1 << DOR_N(TLM_USART)) | (1 << UPE_N(TLM_USART))))
      {
        // discard buffer and start fresh on any error
        parseTelemFrskyByte(START_STOP); // reset
      }
    else
      {
        parseTelemFrskyByte(data);
      }

  }
  UCSRB_N(TLM_USART) |= (1 << RXCIE_N(TLM_USART)); // enable Interrupt
}
#endif

// USART0 Transmit Data Register Empty ISR (UDR was loaded in Shift Register)
ISR(USART_UDRE_vect_N(TLM_USART))
{
  if (Usart0TxBufferCount > 0) {
    UDR_N(TLM_USART) = Usart0TxBuffer[--Usart0TxBufferCount];
  }
  else {
    UCSRB_N(TLM_USART) &= ~(1 << UDRIE_N(TLM_USART)); // Disable UDRE interrupt.
  }
}