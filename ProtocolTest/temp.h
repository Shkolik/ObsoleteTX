/*
 * temp.h
 *
 * Created: 2/2/2020 3:58:58 PM
 *  Author: andrew.shkolik
 */ 


#ifndef TEMP_H_
#define TEMP_H_


/*

Proto_struct Protos[] = {
	{ PROTOCOL_PPM, PROTO_PPM_Cmds },
	//{ PROTOCOL_DSM_SERIAL, DSM_SERIAL_Cmds },
	//{ PROTOCOL_MULTI, MULTI_Cmds }
};

FORCEINLINE uint8_t pulsesStarted()
{
	return (s_current_protocol != 255);
}


uint8_t runAdc = 1;

uint16_t (*timer_callback)(void);
const void * (*PROTO_Cmds)(enum ProtoCmds);

uint16_t g_tmr1Latency_max;
uint16_t g_tmr1Latency_min = -1;
uint16_t dt;

uint8_t s_current_protocol = PROTOCOL_PPM;

void PROTO_Start_Callback( uint16_t (*cb)())
{
	if(!cb)
	{
		//bink(2);
		return;
	}
	
	// Start protocol callback in 16ms
	//SCHEDULE_MIXER_END_IN_US(16000U);	// Schedule next Mixer calculations.
	timer_callback = cb;				// timer_callback = pointer to function.
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		RF_TIMER_COMPA_REG = 16000U;	// 1 count is 1us.
	}
	
	TIMSK |= 1<<OCIE1A; // Enable Output Compare A interrupt.
}

void PROTO_Stop_Callback()
{
	TIMSK	&= ~(1<<OCIE1A);	// Disable Output Compare A interrupt.
	timer_callback = NULL;
}

void sendStopPulses()
{
	PROTO_Cmds(PROTOCMD_RESET);
	PROTO_Stop_Callback();
}

void startPulses(enum ProtoCmds Command)
{
	//stop transmit
	PROTO_Stop_Callback();

	//reset module
	//if (pulsesStarted())
	//{
	//PROTO_Cmds(PROTOCMD_RESET);
	//}
	//set protocol
	s_current_protocol = PROTOCOL_PPM;
	//get callbacks
	PROTO_Cmds = *Protos[s_current_protocol].Cmds;
	//run command
	PROTO_Cmds(Command);
}
*/


#endif /* TEMP_H_ */