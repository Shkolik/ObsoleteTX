/*
 * dx6i.cpp
 *
 * Created: 1/24/2020 9:31:41 PM
 *  Author: Andrew
 */ 
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "../../pgmtypes.h"
#include "dx6i.h"
#include "../../keys.h"

void boardInit()
{
	// Set up I/O port data directions and initial states (unused pin setting : input, pull-up on)
	DDRA = 0b11111111;  PORTA = 0b00000000; // LCD data
	//DDRB = 0b11110111;  PORTB = 0b10101111; // 7:MegaCore Led, 6:PPM_OUT, 5:PPMSIM_OUT, 4:BT_Key_Cmd, SDCARD[3:MISO 2:MOSI 1:SCK 0:CS]
	//DDRC = 0b11111100;  PORTC = 0b00000011; // 7-3:LCD, 2:BackLight, 1:ID2_SW, 0:ID1_SW
	DDRD = 0b11111101;  PORTD = 0b00000010; // 7:AilDR_SW, 6:N/A, 5:N/A, 4:PPM_IN, 3:BT_TX, 2:BT_RX, 1:BIND, 0:PPM_OUT
	DDRE = 0b00010010;  PORTE = 0b11111110; // 7:Flaps, 6:Rot_Left, 5:Rot_Right, 4:Rot_Enter, 3:Trainer_ON, 2:Power_ON, 1:TELEM_TX, 0:TELEM_RX
	DDRF = 0b00000000;  PORTF = 0b11111111; // 7-0:Analog inputs
	//DDRG = 0b00010000;  PORTG = 0b11101111; // 7:N/A, 6:N/A, 5:Rot_1_Push, 4:BT_On_Off, 3:N/A, 2:TCut_SW, 1:Gear_SW, 0: RudDr_SW
	//DDRH = 0b01111110;  PORTH = 0b10010000; // 7:N/A, 6:Sim_Control/RF_Power, 5:Haptic, 4:Hold_Power, 3:Speaker, 2:SPI_Xmitter_SCK, 1:SPI_Xmitter_MOSI, 0:SPI_Xmitter_MISO
	//DDRJ = 0b01111010;  PORTJ = 0b11111111; // 7:N/A, 6:CS_A7105, 5:CS_NRF24L01, 4:CS_CYRF6936, 3:CS_CC2500, 2:N/A, 1:JQ6500Data , 0:JQ6500Busy
	//DDRK = 0b00000000;  PORTK = 0b00000000; // Analogic input (no pull-ups)
	//DDRL = 0b00000000;  PORTL = 0b11111111; // 7:TRN_SW 6:EleDR_SW, 5:ESC, 4:MENU 3:Keyb_Left, 2:Keyb_Right, 1:Keyb_Up, 0:Keyb_Down
	
	adcInit();	
	
	setupTimers();
}

inline void setupTimers()
{
	/**** Set up timer/counter 0 ****/
	// TCNT  10ms = 8MHz/1024/78(.25) periodic timer (10ms interval)
	// TCNT  10ms = 16MHz/1024/156(.25) periodic timer (10ms interval)
	// cycles at 9.984ms but includes 1:4 duty cycle correction to /79 to average at 10.0ms
	TCCR0  = (7 << CS00);								// Norm mode, clk/1024 (1 << CS00 | 1 << CS01 | 1 << CS02
	OCR0   = 78;										// count to 10ms
	TIMSK = (1<<OCIE0) | (1<<TOIE0);					// COMP - 78 (100Hz), OVF - 255 (30.64Hz)
	
	///**** Set up timer/counter 1 ****/
	//TCCR1A = (0<<WGM10);
	//TCCR1B = (1 << WGM12) | (2<<CS10);					// CTC OCR1A, 16MHz / 8
	//
	///**** Set up timer/counter 2 ****/
	//TCCR2 = (2 << CS00);								// Norm mode, clk/8
	//TIMSK |= (1<<TOIE2) ;
	//
	///**** Set up timer/counter 3 ****/
	//TCCR3A = 0;
	//TCCR3B = (1<<ICNC3) | (2<<CS30);					// ICNC3 16MHz / 8
	//TCCR3C = 0;	
	//ETIMSK = (1<<TICIE3);
}

//enable immediately before main loop
inline void enableTimer1()
{
	TIMSK |= (1<<OCIE1A);
}


uint8_t switchState(EnumKeys enuk)
{
	uint8_t result = 0 ;

	//if (enuk < NUM_KEYS)
	//return keys[enuk].state() ? 1 : 0;
//
	//switch(enuk) {
		//case SW_ELE:
		//result = !(PINL & INP_L_ElevDR);
		//break;
//
		//case SW_AIL:
		//result = !(PIND & INP_D_AileDR);
		//break;
//
		//case SW_RUD:
		//result = !(PING & INP_G_RuddDR);
		//break;
//
		////       INP_C_ID1  INP_C_ID2
		//// ID0      0          1
		//// ID1      1          1
		//// ID2      1          0
		//case SW_ID0:
		//result = !(PINC & INP_C_ID1);
		//break;
//
		//case SW_ID1:
		//result = (PINC & INP_C_ID1) && (PINC & INP_C_ID2);
		//break;
//
		//case SW_ID2:
		//result = !(PINC & INP_C_ID2);
		//break;
//
		//case SW_GEA:
		//result = !(PING & INP_G_Gear);
		//break;
//
		//case SW_THR:
		//result = !(PING & INP_G_ThrCt);
		//break;
//
		//case SW_TRN:
		//result = !(PINL & INP_L_Trainer);
		//break;
//
		//default:
		//break;
	//}

	return result;
}

// Trim switches
#if defined(TOGGLETRIM) //Toggle trim usage -> Left trim for right stick and right trim for left stick
static const pm_uchar crossTrim[] PROGMEM = {
	TRIMS_GPIO_PIN_RHL,
	TRIMS_GPIO_PIN_RHR,
	TRIMS_GPIO_PIN_RVD,
	TRIMS_GPIO_PIN_RVU,
	TRIMS_GPIO_PIN_LVD,
	TRIMS_GPIO_PIN_LVU,
	TRIMS_GPIO_PIN_LHL,
	TRIMS_GPIO_PIN_LHR
};
#else
static const pm_uchar crossTrim[] PROGMEM = {
	TRIMS_GPIO_PIN_LHL,
	TRIMS_GPIO_PIN_LHR,
	TRIMS_GPIO_PIN_LVD,
	TRIMS_GPIO_PIN_LVU,
	TRIMS_GPIO_PIN_RVD,
	TRIMS_GPIO_PIN_RVU,
	TRIMS_GPIO_PIN_RHL,
	TRIMS_GPIO_PIN_RHR
};
#endif

uint8_t trimDown(uint8_t idx)
{
	uint8_t in = 0;//~PINF;
	return (in & pgm_read_byte_near(crossTrim+idx));
}

uint8_t keyDown()
{
	return ROTENC_DOWN();
}

void readKeysAndTrims()
{

	//#if defined(NAVIGATION_STICKS)
	//if (~PINL & (KEYS_GPIO_PIN_MENU | KEYS_GPIO_PIN_EXIT)) {   //Check menu key
		//StickScrollTimer = STICK_SCROLL_TIMEOUT;
		//uint8_t sticks_evt = getSticksNavigationEvent();
		//if (sticks_evt) {
			//if (~PINL & KEYS_GPIO_PIN_MENU) {
				//putEvent(EVT_KEY_LONG(sticks_evt)); // create a stick based event "long" to choose menu
				//} else {
				//putEvent(EVT_KEY_BREAK(sticks_evt)); // create a stick based event "first" to choose view (EXIT pressed)
				//killEvents(KEY_EXIT); // Kill exit event
			//}
			//return;
		//}
	//}
	//#endif
//
	//uint8_t enuk = KEY_MENU;
//
	//keys[BTN_REa].input(REA_DOWN());
	//keys[BTN_REb].input(REB_DOWN());
//
	//uint8_t tin = ~PINL;
	//uint8_t in;
	//in = (tin & 0x0f) << 3;
	//in |= (tin & 0x30) >> 3;
//
	//for (uint8_t i=1; i<7; i++) {
		//keys[enuk].input(in & (1<<i));
		//++enuk;
	//}
//
	//// Trims
	//in = ~PINF;
	//for (uint8_t i=0; i<8; i++) {
		//// INP_D_TRM_RH_UP   0 .. INP_D_TRM_LH_UP   7
		//keys[enuk].input(in & pgm_read_byte_near(crossTrim+i));
		//++enuk;
	//}
}

// Rotary encoders increment/decrement

void debounceRotEncA()
{
	DISABLEROTENCAISR();
	rotEncADebounce = ROTENCDEBOUNCEVAL;
}


#if ROTARY_ENCODERS > 0
uint8_t rotEncADebounce;
#endif
#if ROTARY_ENCODERS > 1
uint8_t rotEncBDebounce;
#endif

void debounceRotEncB()
{
DISABLEROTENCAISR();
rotEncADebounce = ROTENCDEBOUNCEVAL;
	//DISABLEROTENCBISR();
	//rotEncBDebounce = ROTENCDEBOUNCEVAL;
}
