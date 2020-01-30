/*
 * misc.c
 *
 * Created: 1/25/2020 3:42:20 PM
 *  Author: Andrew
 */ 
#include "../ObsoleteTX.h"

extern volatile uint8_t g_sync_count;
static volatile uint32_t msecs = 0;

void inline PROTO_Change_Callback(uint16_t (*cb)())
{
  /*
   * Change the pointer to the callback function.
   * Should be called from within a protocol callback function.
   * e.g. To go from bind to normal mode.
   * skyartec_bind_cb() calls this ... Change_Callback(skyartec_norm_cb);
  */
  if(! cb) return;
  timer_callback = cb; // timer_callback = pointer to function.
}


void PROTO_Start_Callback( uint16_t (*cb)())
{
  if(! cb) return;
  // Start protocol callback in 16 milli-seconds.
  SCHEDULE_MIXER_END_IN_US(16000U); // Schedule next Mixer calculations.
  timer_callback = cb; // timer_callback = pointer to function.
//
  //ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    //RF_TIMER_COMPA_REG = TCNT1 + (16000U *2); // 1 count is 0.5us.
  //}
//
  //TIFR1 |= 1<<OCF1A; // Reset Flag.
  //TIMSK1 |= 1<<OCIE1A; // Enable Output Compare A interrupt.
}


void PROTO_Stop_Callback()
{
  //TIMSK1 &= ~(1<<OCIE1A); // Disable Output Compare A interrupt.
  //TIFR1 |= 1<<OCF1A; // Reset Flag.
  //timer_callback = NULL;
}


uint32_t CLOCK_getms()
{
  uint32_t ms;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    ms = msecs;
  }
  return ms;
}


void CLOCK_delayms(uint32_t delay_ms)
{
  uint32_t start_ms;

  start_ms = msecs;
  while(msecs < (start_ms + delay_ms));
}


void PROTOCOL_SetBindState(uint16_t t10ms)
{
  if(t10ms) 
  {
		protoMode = BIND_MODE;
		Bind_tmr10ms = t10ms;
  }
  else protoMode = NORMAL_MODE;
}

#if defined(SPIMODULES)
uint16_t RFPowerOut = 0;
uint8_t * packet = pulses2MHz.pbyte; //protocol global packet (Use 40 MAX)
uint8_t * channel_used = &pulses2MHz.pbyte[CHANNEL_USED_OFFSET]; //protocol global channel (Use 50 MAX -> 54 bytes free to use in SPI protocols)
uint8_t * temp_rfid_addr = &pulses2MHz.pbyte[RX_TX_ADDR_OFFSET];
#if defined(PROTO_HAS_CYRF6936)
//uint8_t * cyrfmfg_id = &pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-30]; // -30 to -25 cyrfmfg_id[6] used in DSM, DEVO
#endif
#if defined(PROTO_HAS_CC2500) || defined(PROTO_HAS_CYRF6936)
uint8_t * telem_save_data = &pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-40]; // used in [9] FrskyX & [10] DSM telemetry
#endif
#if defined(PROTO_HAS_CC2500)
uint8_t calData[48]; // used in FrskyX protocol
uint32_t seed; // used in FrskyV telemetry
#endif
#endif