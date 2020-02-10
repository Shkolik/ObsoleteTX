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

#define START_STOP                        0x7e
#define BYTESTUFF                         0x7d
#define STUFF_MASK                        0x20

#define NUM_TELEM_RX_BUFFER  2
#define TELEM_RX_PACKET_SIZE 9    // Frsky packet size
#define USART_TX_PACKET_SIZE 26   // used in Multiprotocol serial

extern uint8_t TelemetryRxBuffer[NUM_TELEM_RX_BUFFER][TELEM_RX_PACKET_SIZE];

extern uint8_t * UsartTxBuffer; // [USART_TX_PACKET_SIZE] used

extern uint8_t UsartTxBufferCount;

///////////  Telemetry fields  ///////////////
#if defined(TELEMETRY)

void LoadTelemBuffer(uint8_t *data);

#define RAW_FRSKY_MINMAX(v)       v.value
#define IF_GPS_IS_FIXED           if (telemetryData.value.gpsFix)
#define IF_GPS_IS_NOT_FIXED       if (!telemetryData.value.gpsFix)
#define NUMCELLS 6

class TelemetryValueWithMin
{
	public:
	uint8_t value;
	uint8_t min;
	void set(uint8_t value);
};

class TelemetryValueWithMinMax: public TelemetryValueWithMin
{
	public:
	uint8_t max;
	void set(uint8_t value, uint8_t unit);
};

struct TelemetrySerialData {
	int16_t  baroAltitudeOffset;
	int16_t  gpsAltitude;
	int16_t  temperature1;
	uint16_t rpm;
	uint16_t fuelLevel;
	int16_t  temperature2;
	uint16_t volts;
	uint32_t distFromEarthAxis;
	uint8_t  cellVolts[NUMCELLS];
	int16_t  baroAltitude;
	uint16_t gpsSpeed_bp;
	uint16_t gpsLongitude_bp;
	uint16_t gpsLatitude_bp;
	uint16_t gpsCourse_bp;
	uint8_t  day;
	uint8_t  month;
	uint8_t  year;
	uint8_t  hour;
	uint8_t  min;
	uint8_t  sec;
	uint16_t gpsSpeed_ap;
	uint16_t gpsLongitude_ap;
	uint16_t gpsLatitude_ap;
	uint16_t gpsCourse_ap;
	uint32_t pilotLatitude;
	uint32_t pilotLongitude;
	uint8_t  gpsLongitudeEW;
	uint8_t  gpsLatitudeNS;
	int16_t  accelX;
	int16_t  accelY;
	int16_t  accelZ;
	uint8_t  notUsed:1;         // to use
	uint8_t  gpsFix:1;          // 0=not fixed, 1=fixed
	uint8_t  gpsGetFirstData:1; // Used to wait another GPS data frame before setting gpsFix to 1;
	uint8_t  openXsensor:1;
	uint8_t  cellsCount:4;
	uint8_t  minCellVolts;
	uint16_t current;
	int16_t  varioSpeed;

	uint16_t gpsDistance;
	int16_t  gpsAltitudeOffset;
	uint8_t  minCellIdx;
	int16_t  cellsSum;
	uint16_t currentConsumption;
	uint16_t currentPrescale;
	uint16_t power;
	int16_t  airSpeed;
	uint16_t vfas;
	uint16_t volts_bp;
	uint16_t volts_ap;
	// end of FrSky Hub data + OXS

	/* next fields must keep this order! */
	int16_t  minAltitude;
	int16_t  maxAltitude;
	uint16_t maxRpm;
	int16_t  maxTemperature1;
	int16_t  maxTemperature2;
	uint16_t maxGpsSpeed;
	uint16_t maxGpsDistance;
	uint16_t maxAirSpeed;
	int16_t  minCell;
	int16_t  minCells;
	uint16_t minVfas;
	uint16_t maxCurrent;
	uint16_t maxPower;
	/* end */

	int16_t  dTE;
} __attribute__((__packed__));

enum TelemAnas {
	TELEM_ANA_A1,
	TELEM_ANA_A2,
	TELEM_ANA_COUNT
};

struct TelemetryData {
	TelemetryValueWithMinMax analog[TELEM_ANA_COUNT];
	TelemetryValueWithMin    rssi[2];
	TelemetrySerialData value;
} __attribute__((__packed__));
#endif

#endif /* USART_DRIVER_H_ */