/*
 * boerd.h
 *
 * Created: 1/24/2020 9:17:19 PM
 *  Author: Andrew
 */ 


#ifndef BOARD_H_
#define BOARD_H_


//Port Pin Bitmaps for general use.

#ifndef PIN0_bm
#define PIN0_bm  0x01
#endif
#ifndef PIN1_bm
#define PIN1_bm  0x02
#endif
#ifndef PIN2_bm
#define PIN2_bm  0x04
#endif
#ifndef PIN3_bm
#define PIN3_bm  0x08
#endif
#ifndef PIN4_bm
#define PIN4_bm  0x10
#endif
#ifndef PIN5_bm
#define PIN5_bm  0x20
#endif
#ifndef PIN6_bm
#define PIN6_bm  0x40
#endif
#ifndef PIN7_bm
#define PIN7_bm  0x80
#endif

//END: Port Pin Bitmaps for general use.

// ADC driver
extern void adcInit();
extern void getADC();

//USART driver (static register dispatcher)

#define RXD_DDR1 DDRD
#define RXD_DDR_PIN1 DDD2
#define RXD_PORT1 PORTD
#define RXD_PORT_PIN1 PORTD2
#define RXD_DDR0 DDRE
#define RXD_DDR_PIN0 DDE0
#define RXD_PORT0 PORTE
#define RXD_PORT_PIN0 PORTE0

#define _DOR_N(usart_no) DOR ## usart_no
#define _FE_N(usart_no) FE ## usart_no
#define _RXCIE_N(usart_no) RXCIE ## usart_no
#define _RXC_N(usart_no) RXC ## usart_no
#define _RXD_DDR_N(usart_no) RXD_DDR ## usart_no
#define _RXD_DDR_PIN_N(usart_no) RXD_DDR ## usart_no
#define _RXD_PORT_N(usart_no) RXD_DDR ## usart_no
#define _RXD_PORT_PIN_N(usart_no) RXD_DDR ## usart_no
#define _RXEN_N(usart_no) RXEN ## usart_no
#define _TXCIE_N(usart_no) TXCIE ## usart_no
#define _TXEN_N(usart_no) TXEN ## usart_no
#define _U2X_N(usart_no) U2X ## usart_no
#define _UBRRH_N(usart_no) UBRR ## usart_no ## H
#define _UBRRL_N(usart_no) UBRR ## usart_no ## L
#define _UCSRA_N(usart_no) UCSR ## usart_no ## A
#define _UCSRB_N(usart_no) UCSR ## usart_no ## B
#define _UCSRC_N(usart_no) UCSR ## usart_no ## C
#define _UCSZ0_N(usart_no) UCSZ ## usart_no ## 0
#define _UCSZ1_N(usart_no) UCSZ ## usart_no ## 1
#define _UCSZ2_N(usart_no) UCSZ ## usart_no ## 2
#define _UDRIE_N(usart_no) UDRIE ## usart_no
#define _UDR_N(usart_no) UDR ## usart_no
#define _UPE_N(usart_no) UPE ## usart_no
#define _USART_RX_vect_N(usart_no) USART ## usart_no ## _RX_vect
#define _USART_UDRE_vect_N(usart_no) USART ## usart_no ## _UDRE_vect

#define DOR_N(usart_no) _DOR_N(usart_no)
#define FE_N(usart_no) _FE_N(usart_no)
#define RXCIE_N(usart_no) _RXCIE_N(usart_no)
#define RXC_N(usart_no) _RXC_N(usart_no)
#define RXD_DDR_N(usart_no) _RXD_DDR_N(usart_no)
#define RXD_DDR_PIN_N(usart_no) _RXD_DDR_PIN_N(usart_no)
#define RXD_PORT_N(usart_no) _RXD_PORT_N(usart_no)
#define RXD_PORT_PIN_N(usart_no) _RXD_PORT_PIN_N(usart_no)
#define RXEN_N(usart_no) _RXEN_N(usart_no)
#define TXCIE_N(usart_no) _TXCIE_N(usart_no)
#define TXEN_N(usart_no) _TXEN_N(usart_no)
#define U2X_N(usart_no) _U2X_N(usart_no)
#define UBRRH_N(usart_no) _UBRRH_N(usart_no)
#define UBRRL_N(usart_no) _UBRRL_N(usart_no)
#define UCSRA_N(usart_no) _UCSRA_N(usart_no)
#define UCSRB_N(usart_no) _UCSRB_N(usart_no)
#define UCSRC_N(usart_no) _UCSRC_N(usart_no)
#define UCSZ0_N(usart_no) _UCSZ0_N(usart_no)
#define UCSZ1_N(usart_no) _UCSZ1_N(usart_no)
#define UCSZ2_N(usart_no) _UCSZ2_N(usart_no)
#define UDRIE_N(usart_no) _UDRIE_N(usart_no)
#define UDR_N(usart_no) _UDR_N(usart_no)
#define UPE_N(usart_no) _UPE_N(usart_no)
#define USART_RX_vect_N(usart_no) _USART_RX_vect_N(usart_no)
#define USART_UDRE_vect_N(usart_no) _USART_UDRE_vect_N(usart_no)

//#define USART_RX_vect USART0_RX_vect
//END: USART driver (static register dispatcher)

#endif /* BOARD_H_ */