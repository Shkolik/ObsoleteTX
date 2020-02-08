/*
 * telemetry.cpp
 *
 * Created: 2/5/2020 11:53:02 PM
 *  Author: andrew.shkolik
 */ 

#include "telemetry.h"

uint8_t multiBindStatus = MULTI_NORMAL_OPERATION;
MultiModuleStatus multiModuleStatus;
MultiModuleSyncStatus multiSyncStatus;

uint8_t telemetryRxBuffer[TELEMETRY_RX_PACKET_SIZE];
uint8_t telemetryRxBufferCount;

enum MultiPacketTypes  {
	MultiStatus = 1,
	FrSkySportTelemtry,
	FrSkyHubTelemetry,
	SpektrumTelemetry,
	DSMBindPacket,
	FlyskyIBusTelemetry,
	ConfigCommand,
	InputSync,
	FrskySportPolling
};

enum MultiBufferState  {
	NoProtocolDetected,
	MultiFirstByteReceived,
	ReceivingMultiProtocol,
	ReceivingMultiStatus,
	SpektrumTelemetryFallback,
	FrskyTelemetryFallback,
	FrskyTelemetryFallbackFirstByte,
	FrskyTelemetryFallbackNextBytes,
	FlyskyTelemetryFallback,
	MultiStatusOrFrskyData
};

NOINLINE void processFrskyTelemetryData(uint8_t data)
{
	static uint8_t dataState = STATE_DATA_IDLE;

	switch (dataState) {
		case STATE_DATA_START:
		//if (data == START_STOP) {
			//if (IS_FRSKY_SPORT_PROTOCOL()) {
				//dataState = STATE_DATA_IN_FRAME ;
				//telemetryRxBufferCount = 0;
			//}
		//}
		//else {
			//if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
				//telemetryRxBuffer[telemetryRxBufferCount++] = data;
			//}
			//dataState = STATE_DATA_IN_FRAME;
		//}
		break;

		case STATE_DATA_IN_FRAME:
		if (data == BYTESTUFF) {
			dataState = STATE_DATA_XOR; // XOR next byte
		}
		else if (data == START_STOP) {
			//if (IS_FRSKY_SPORT_PROTOCOL()) {
				//dataState = STATE_DATA_IN_FRAME ;
				//telemetryRxBufferCount = 0;
			//}
			//else {
				// end of frame detected
				//frskyDProcessPacket(telemetryRxBuffer);
				dataState = STATE_DATA_IDLE;
			//}
			//break;
		}
		else if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
			telemetryRxBuffer[telemetryRxBufferCount++] = data;
		}
		break;

		case STATE_DATA_XOR:
		if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
			telemetryRxBuffer[telemetryRxBufferCount++] = data ^ STUFF_MASK;
		}
		dataState = STATE_DATA_IN_FRAME;
		break;

		case STATE_DATA_IDLE:
		if (data == START_STOP) {
			telemetryRxBufferCount = 0;
			dataState = STATE_DATA_START;
		}
		
		break;

		
	} // switch

	//#if defined(TELEMETRY_FRSKY_SPORT)
	//if (IS_FRSKY_SPORT_PROTOCOL() && telemetryRxBufferCount >= FRSKY_SPORT_PACKET_SIZE) {
		//sportProcessPacket(telemetryRxBuffer);
		//dataState = STATE_DATA_IDLE;
	//}
	//#endif
}

static MultiBufferState multiTelemetryBufferState;

static void processMultiStatusPacket(const uint8_t *data)
{
	// At least two status packets without bind flag
	bool wasBinding = (multiModuleStatus.flags & 0x08);

	multiModuleStatus.flags = data[0];
	multiModuleStatus.major = data[1];
	multiModuleStatus.minor = data[2];
	multiModuleStatus.revision = data[3];
	multiModuleStatus.patch = data[4];
	multiModuleStatus.lastUpdate = g_tmr10ms;

	if (wasBinding && !(multiModuleStatus.flags & 0x08) && multiBindStatus == MULTI_BIND_INITIATED)
	multiBindStatus = MULTI_BIND_FINISHED;
}

static void processMultiSyncPacket(const uint8_t *data)
{
	multiSyncStatus.lastUpdate = g_tmr10ms;
	multiSyncStatus.interval = data[4];
	multiSyncStatus.target = data[5];
	

	//multiSyncStatus.calcAdjustedRefreshRate(data[0] << 8 | data[1], data[2] << 8 | data[3]);

	
}


static void processMultiTelemetryPaket(const uint8_t *packet)
{
	uint8_t type = packet[0];
	uint8_t len = packet[1];
	const uint8_t *data = packet + 2;

	// Switch type
	switch (type) {
		case MultiStatus:
		if (len >= 5)
		processMultiStatusPacket(data);
		break;
		case DSMBindPacket:
		if (len >= 10)
		//processDSMBindPacket(data);
		break;
		case SpektrumTelemetry:
		// processSpektrumPacket expects data[0] to be the telemetry indicator 0xAA but does not check it,
		// just send one byte of our header instead
		if (len >= 17){}
		//processSpektrumPacket(data - 1);
		else
		{
			
		Serial0.println("[MP] Received spektrum telemetry len %d < 17");
		Serial0.println(len);
		}
		break;
		case FlyskyIBusTelemetry:
		if (len >= 28){}
		//processFlySkyPacket(data);
		else
		{
			Serial0.println("[MP] Received IBUS telemetry len %d < 28");
		Serial0.println(len);
		}
		break;
		case FrSkyHubTelemetry:
		if (len >= 4){}
		//frskyDProcessPacket(data);
		else
		{Serial0.println("[MP] Received Frsky HUB telemetry len %d < 4");
		Serial0.println(len);}
		break;
		case FrSkySportTelemtry:
		if (len >= 4)
		{
			
		}
		//sportProcessTelemetryPacket(data);
		else
		{Serial0.println("[MP] Received sport telemetry len %d < 4");
		Serial0.println(len);}
		break;
		case InputSync:
		if (len >= 6)
		processMultiSyncPacket(data);
		else
		{Serial0.println("[MP] Received input sync len %d < 6");
		Serial0.println(len);}
		break;
		case ConfigCommand:
		// Just an ask to our command, ignore for now
		break;
		case FrskySportPolling:
		
		break;
		default:
		{Serial0.println("[MP] Unknown multi packet type 0x%02X, len %d");
		Serial0.println(type);
		Serial0.println(len);}
		
		break;
	}
}


static void processMultiTelemetryByte(const uint8_t data)
{
	if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
		telemetryRxBuffer[telemetryRxBufferCount++] = data;
	}
	else {
		Serial0.println("[MP] array size %d error");
		Serial0.println(telemetryRxBufferCount);
		multiTelemetryBufferState = NoProtocolDetected;
	}

	// Length field does not count the header
	if (telemetryRxBufferCount >= 2 && telemetryRxBuffer[1] == telemetryRxBufferCount - 2) {
		// debug print the content of the packet
		// Packet is complete, process it
		processMultiTelemetryPaket(telemetryRxBuffer);
		multiTelemetryBufferState = NoProtocolDetected;
	}
}

MultiBufferState guessProtocol()
{
	if (g_model.rfOptionValue1 == MM_RF_PROTO_DSM2)
	return SpektrumTelemetryFallback;
	else if (g_model.rfOptionValue1 == MM_RF_PROTO_FS_AFHDS2A)
	return FlyskyTelemetryFallback;
	else
	return FrskyTelemetryFallback;
}

void processMultiTelemetryData(const uint8_t data)
{
	// debugPrintf("State: %d, byte received %02X, buflen: %d\r\n", multiTelemetryBufferState, data, telemetryRxBufferCount);
	switch (multiTelemetryBufferState) {
		case NoProtocolDetected:
		if (data == 'M') {
			multiTelemetryBufferState = MultiFirstByteReceived;
		}
		else if (data == 0xAA || data == 0x7e) {
			multiTelemetryBufferState = guessProtocol();

			// Process the first byte by the protocol
			processMultiTelemetryData(data);
		}
		else {
			Serial0.println("[MP] invalid start byte 0x%02X");
			Serial0.println(data);
		}
		break;

		case FrskyTelemetryFallback:
		multiTelemetryBufferState = FrskyTelemetryFallbackFirstByte;
		//processFrskyTelemetryData(data);
		break;

		case FrskyTelemetryFallbackFirstByte:
		if (data == 'M') {
			multiTelemetryBufferState = MultiStatusOrFrskyData;
		}
		else {
			//processFrskyTelemetryData(data);
			if (data != 0x7e)
			multiTelemetryBufferState = FrskyTelemetryFallbackNextBytes;
		}

		break;

		case FrskyTelemetryFallbackNextBytes:
		//processFrskyTelemetryData(data);
		if (data == 0x7e)
		// end of packet or start of new packet
		multiTelemetryBufferState = FrskyTelemetryFallbackFirstByte;
		break;

		case FlyskyTelemetryFallback:
		//processFlySkyTelemetryData(data);
		if (telemetryRxBufferCount == 0)
		multiTelemetryBufferState = NoProtocolDetected;
		break;

		case SpektrumTelemetryFallback:
		//processSpektrumTelemetryData(data);
		if (telemetryRxBufferCount == 0)
		multiTelemetryBufferState = NoProtocolDetected;
		break;

		case MultiFirstByteReceived:
		telemetryRxBufferCount = 0;
		if (data == 'P') {
			multiTelemetryBufferState = ReceivingMultiProtocol;
		}
		else if (data >= 5 && data <= 10) {
			// Protocol indented for er9x/ersky9, accept only 5-10 as packet length to have
			// a bit of validation
			multiTelemetryBufferState = ReceivingMultiStatus;
			processMultiTelemetryData(data);
		}
		else {
			Serial0.println("[MP] invalid second byte 0x%02X");
			Serial0.println(data);
			multiTelemetryBufferState = NoProtocolDetected;
		}
		break;

		case ReceivingMultiProtocol:
		processMultiTelemetryByte(data);
		break;

		case MultiStatusOrFrskyData:
		// Check len byte if it makes sense for multi
		if (data >= 5 && data <= 10) {
			multiTelemetryBufferState = ReceivingMultiStatus;
			telemetryRxBufferCount = 0;
		}
		else {
			multiTelemetryBufferState = FrskyTelemetryFallbackNextBytes;
			processMultiTelemetryData('M');
		}
		processMultiTelemetryData(data);
		break;

		case ReceivingMultiStatus:
		telemetryRxBuffer[telemetryRxBufferCount++] = data;
		if (telemetryRxBufferCount > 5 && telemetryRxBuffer[0] == telemetryRxBufferCount-1) {
			processMultiStatusPacket(telemetryRxBuffer+1);
			telemetryRxBufferCount = 0;
			multiTelemetryBufferState = NoProtocolDetected;
		}
		if (telemetryRxBufferCount > 10) {
			// too long ignore
			Serial0.println("Overlong multi status packet detected ignoring, wanted %d");
			Serial0.println(telemetryRxBuffer[0]);
			telemetryRxBufferCount =0;
			multiTelemetryBufferState = NoProtocolDetected;
		}


	}

}

