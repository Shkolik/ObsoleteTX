/*
 * telemetry.h
 *
 * Created: 2/5/2020 11:56:18 PM
 *  Author: andrew.shkolik
 */ 


#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include "main.h"

#define TELEMETRY_RX_PACKET_SIZE       128


extern uint8_t telemetryRxBuffer[TELEMETRY_RX_PACKET_SIZE];
extern uint8_t telemetryRxBufferCount;

void processMultiTelemetryData(uint8_t data);

enum FrSkyDataState {
	STATE_DATA_IDLE,
	STATE_DATA_START,
	STATE_DATA_IN_FRAME,
	STATE_DATA_XOR,
	
};

 //This should be put into the Module definition if other modules gain this functionality
struct MultiModuleSyncStatus {
	uint32_t adjustedRefreshRate;    // 
	uint16_t lastUpdate;
	uint16_t refreshRate;
	uint16_t inputLag;
	uint8_t interval;
	uint8_t target;

	//bool isValid() {return (g_tmr10ms  - lastUpdate < 100);}
	//void getRefreshString(char* refreshText);
	//uint16_t getAdjustedRefreshRate();
	//void calcAdjustedRefreshRate(uint16_t newRefreshRate, uint16_t newInputLag);
//
	//MultiModuleSyncStatus() {
		//// Initialize to a valid value
		//adjustedRefreshRate=9000 * 1000;
	//}

};

extern MultiModuleSyncStatus multiSyncStatus;


struct MultiModuleStatus {

	uint8_t major;
	uint8_t minor;
	uint8_t revision;
	uint8_t patch;

	uint8_t flags;
	uint16_t lastUpdate;

	//void getStatusString(char* statusText);
//
	//bool isValid() { return (bool)(g_tmr10ms - lastUpdate < 200); }
	//inline bool supportsFailsafe() { return (bool) (flags & 0x20); }
	//inline bool isWaitingforBind() { return (bool) (flags & 0x10); }
	//inline bool isBinding() { return (bool) (flags & 0x08); }
	//inline bool protocolValid() { return (bool) (flags & 0x04); }
	//inline bool serialMode() { return (bool) (flags & 0x02); }
	//inline bool inputDetected() { return (bool) (flags & 0x01); }
};

extern MultiModuleStatus multiModuleStatus;
enum MultiBindStatus  {
	MULTI_NORMAL_OPERATION,
	MULTI_BIND_INITIATED,
	MULTI_BIND_FINISHED,
};

extern uint8_t multiBindStatus;

#endif /* TELEMETRY_H_ */