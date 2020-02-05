/*
 * usart_driver.h
 *
 * Created: 2/5/2020 10:17:03 AM
 *  Author: andrew.shkolik
 */ 


#ifndef USART_DRIVER_H_
#define USART_DRIVER_H_
#include "main.h"


#define TLM_USART 1

void UsartTransmitBuffer();
void UsartEnableTx();
void UsartEnableRx();
void UsartDisableTx();
void UsartDisableRx();
void UsartSet8N1();
void UsartSet8E2();
void UsartSet9600BAUDS();    //Frsky HUB telemetry
void UsartSet57600BAUDS();   //Frsky S.port telemetry
void UsartSet125000BAUDS();  //DSM Serial protocol
void UsartSet100000BAUDS();  //Multiprotocol Serial

#define START_STOP              0x7e

#define NUM_TELEM_RX_BUFFER  2
#define TELEM_RX_PACKET_SIZE 9    // Frsky packet size
#define USART_TX_PACKET_SIZE 26   // used in Multiprotocol serial

extern uint8_t TelemetryRxBuffer[NUM_TELEM_RX_BUFFER][TELEM_RX_PACKET_SIZE];

extern uint8_t * UsartTxBuffer; // [USART_TX_PACKET_SIZE] used

extern uint8_t UsartTxBufferCount;



#endif /* USART_DRIVER_H_ */