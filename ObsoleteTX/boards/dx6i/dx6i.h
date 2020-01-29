/*
 * dx6i.h
 *
 * Created: 1/24/2020 9:31:31 PM
 *  Author: Andrew
 */ 


#ifndef DX6I_H_
#define DX6I_H_

#include "../board.h"

#define NUM_STICKS	4
#define CALCULATE_LAT_JIT()  dt = TCNT1 - OCR1A		// Calculate latency and jitter.
#define RF_TIMER_COMPA_REG   OCR1A
#define RF_TIMER_COMPA_VECT  TIMER1_COMPA_vect

#define TIMER_10MS_VECT           TIMER0_COMP_vect //10ms timer
#define TIMER_10MS_COMPVAL        OCR0

#define MEGACORELEDPIN			PIN4_bm

inline void setupTimers();
inline void enableTimer1();

#ifdef EXTERNALEEPROM
#define ADDRESS_EXTERN_EEPROM  (0x50 << 1) //0x50 with all strap closed on ZS042 module (or alone), EEPROM FM24W256, see datasheet
#define EEPROMREADBLOCK(a, b, c)   Ext_eeprom_read_block(a, b, c) //External EEPROM
#else
#define EEPROMREADBLOCK(a, b, c)   eeprom_read_block(a, (const void *)b, c) //Internal EEPROM
#endif

// Keys
void readKeysAndTrims();
//#define GPIO_KEYS                 PINE
//#define KEYS_GPIO_PIN_MENU        PIN4_bm //encoder pressed
//#define KEYS_GPIO_PIN_EXIT        PIN7_bm //Get rid of EXIT key
//#define KEYS_GPIO_PIN_RIGHT       PIN5_bm //EncA
//#define KEYS_GPIO_PIN_LEFT        PIN6_bm //EncB
//#define KEYS_GPIO_PIN_UP          PIN7_bm //Get rid of UP key
//#define KEYS_GPIO_PIN_DOWN        PIN7_bm //Get rid of DOWN key
// DBLKeys driver
#define KEYS_PRESSED()            (0)

// Trims
#define GPIO_TRIMS                PING
#define TRIMS_GPIO_PIN_LHL        PIN7_bm
#define TRIMS_GPIO_PIN_LVD        PIN5_bm
#define TRIMS_GPIO_PIN_RVU        PIN2_bm
#define TRIMS_GPIO_PIN_RHL        PIN1_bm
#define TRIMS_GPIO_PIN_LHR        PIN6_bm
#define TRIMS_GPIO_PIN_LVU        PIN4_bm
#define TRIMS_GPIO_PIN_RVD        PIN3_bm
#define TRIMS_GPIO_PIN_RHR        PIN0_bm
#define TRIMS_PRESSED()           (~GPIO_TRIMS)

// Backlight driver
#if !defined(PWM_BACKLIGHT)
#define backlightEnable()         PORTC |= _BV(OUT_C_LIGHT)
#define backlightDisable()        PORTC &= ~_BV(OUT_C_LIGHT)
#define isBacklightEnable()       PORTC & _BV(OUT_C_LIGHT)
#define BACKLIGHT_ON()            PORTC |= _BV(OUT_C_LIGHT)
#define BACKLIGHT_OFF()           PORTC &= ~_BV(OUT_C_LIGHT)
#define EnableCoreLed()           PORTB |= (MEGACORELEDPIN) // Blink on function
#define DisableCoreLed()          PORTB &= ~(MEGACORELEDPIN) // Blink off function
#define LEDON()                   EnableCoreLed()
#define LEDOFF()                  DisableCoreLed()
#else
// OC0A on pin PB7 (pin26) aka MEGACORELEDPIN
#define BACKLIGHT_ON()            (OCR0A = ( (uint8_t) g_eeGeneral.blOnBright)  <<4)
#define BACKLIGHT_OFF()           (OCR0A = ( (uint8_t) g_eeGeneral.blOffBright) <<4)
#define LEDON()
#define LEDOFF()
#endif

//// SD driver
//#define SDCARD_CS_N_ACTIVE()        PORTB &= ~PIN0_bm // MMC CS = L
//#define select_card()               SDCARD_CS_N_ACTIVE()
//#define SDCARD_CS_N_INACTIVE()      PORTB |= PIN0_bm // MMC CS = H
//#define unselect_card()             SDCARD_CS_N_INACTIVE()
//#define SDCARD_CS_N_IS_INACTIVE()   (PINB & PIN0_bm)
//#define SPI_250K() { SPSR = _BV(SPI2X); SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0); }
//#define SPI_4M()   { SPCR = _BV(SPE) | _BV(MSTR); }
//#define SPI_8M()   { SPSR = _BV(SPI2X); SPCR = _BV(SPE) | _BV(MSTR); }
//#define SPI_START_SPEED()           SPI_250K()

// Switchs driver
#define INP_C_ID2                 PIN1_bm
#define INP_C_ID1                 PIN0_bm
#define INP_D_AileDR              PIN7_bm
#define INP_G_ThrCt               PIN2_bm
#define INP_G_Gear                PIN1_bm
#define INP_G_RuddDR              PIN0_bm
#define INP_L_ElevDR              PIN6_bm
#define INP_L_Trainer             PIN7_bm

//// Bluetooth pin
//#define OUT_B_BT_KEY              PIN4_bm    // BT_Key_Cmd
//#define OUT_G_BT_ONOFF            PIN4_bm    // BT_On_Off
//#define BT_KEY_ON()               (PORTB |=  OUT_B_BT_KEY)
//#define BT_KEY_OFF()              (PORTB &= ~OUT_B_BT_KEY)
//#define BT_IS_IN_AT_MODE          (PORTB & OUT_B_BT_KEY)
//#define DIRECTBTWIRES             test
//#ifdef DIRECTBTWIRES
//#define BT_POWER_ON()             (PORTG |=  OUT_G_BT_ONOFF)
//#define BT_POWER_OFF()            (PORTG &= ~OUT_G_BT_ONOFF)
//#else
//#define BT_POWER_ON()             (PORTG &= ~OUT_G_BT_ONOFF)
//#define BT_POWER_OFF()            (PORTG |=  OUT_G_BT_ONOFF)
//#endif
// Servitudes driver
#define INP_D_PPM_IN              4    // ICP1
#define OUT_B_PPM                 6    // Master_PPM_out OC1A
#define OUT_B_PPM16_SIM           5    // OC1B
#define INP_D_I2C_SCL             1
#define INP_D_I2C_SDA             0
#define INP_E_TELEM_RX            1
#define OUT_E_TELEM_TX            0
//#define INP_H_RF_Activated        6
//#define INP_H_DSC_Activated       5    //not used, reserved for pwrCheck()
#define INP_H_Hold_Power          PIN4_bm
#define OUT_H_SpeakerBuzzer       6
#define OUT_H_PPM16_SIM_CTL       6
#define OUT_J_JQ_SERIAL           1    //JQ6500
#define INP_J_JQ_BUSY             0    //JQ6500
#define OUT_H_HAPTIC              5

//Puppy Master Mode
#define ACTIVE_PPM_OUT()            PORTA &= ~_BV(OUT_H_PPM16_SIM_CTL)
#define ACTIVE_PPM_IN()             PORTA |= _BV(OUT_H_PPM16_SIM_CTL)
#define ENABLE_TRAINER_INTERRUPT()  TIMSK |= _BV(ICIE1);  // Enable ICP Interrupt.
#define DISABLE_TRAINER_INTERRUPT() TIMSK &= ~_BV(ICIE1); // Disable ICP Interrupt.
#define WAIT_PUPIL()                ENABLE_TRAINER_INTERRUPT(); ACTIVE_PPM_IN()
#define PPM16_CONF()                DISABLE_TRAINER_INTERRUPT(); ACTIVE_PPM_OUT()
#define TRAINER_TC_VECT             TIMER1_CAPT_vect
#define TRAINER_TC_REG              ICR1

// Rotary encoders driver
#define INP_E_ROT_ENC_1_A         PIN5_bm
#define INP_E_ROT_ENC_1_B         PIN6_bm
#define ROT_ENC_1_MASK            (INP_E_ROT_ENC_1_A | INP_E_ROT_ENC_1_B)
//#define INP_E_ROT_ENC_2_A         PIN7_bm
//#define INP_E_ROT_ENC_2_B         PIN6_bm
//#define ROT_ENC_2_MASK            (INP_E_ROT_ENC_2_A | INP_E_ROT_ENC_2_B)
#define INP_E_ROT_ENC_1_PUSH      PIN4_bm
//#define INP_E_ROT_ENC_2_PUSH      PIN3_bm
#define REA_DOWN()                (~PINE & INP_E_ROT_ENC_1_PUSH)
//#define REB_DOWN()                (~PINE & INP_E_ROT_ENC_2_PUSH)
#define ROTENC_DOWN()             (~PINE & INP_E_ROT_ENC_1_PUSH)//(REA_DOWN() || REB_DOWN())
#define ENABLEROTENCAISR()        EIMSK |= _BV(INT5) | _BV(INT6) // enable the rot. enc.A ext. int.
//#define ENABLEROTENCBISR()        EIMSK |= _BV(INT7) | _BV(INT6) // enable the rot. enc.B ext. int.
#define DISABLEROTENCAISR()       EIMSK &= (~(_BV(INT5) | _BV(INT6))) // disable the rot.A enc. ext. int.
//#define DISABLEROTENCBISR()       EIMSK &= (~(_BV(INT7) | _BV(INT6))) // disable the rot.B enc. ext. int.

// LCD driver
#define PORTA_LCD_DAT            PORTA
#define PORTC_LCD_CTRL           PORTC
#if defined(LCD_KS108)              // (For KS108 LCD only) MEGA R/W pin always at 0 state in OpenAVRc then
#define OUT_C_LCD_CS2			6  // use this pin to control second KS108 (CS2)
#else                               // and connect KS108 R/W pin to ground via a 1k resistor
#define OUT_C_LCD_RnW			6
#endif
#define OUT_C_LCD_E              4
#define OUT_C_LCD_A0             3
#define OUT_C_LCD_RES            2
#define OUT_C_LCD_CS1            1
#define OUT_C_LIGHT              5//figure out

// Power driver
void boardOff();
#if defined(PWRMANAGE) && !defined(SIMU)
#define UNEXPECTED_SHUTDOWN()   ((mcusr & _BV(WDRF)) || g_eeGeneral.unexpectedShutdown)
#else
#define UNEXPECTED_SHUTDOWN()   (mcusr & _BV(WDRF))
#endif

// Haptic driver
#define hapticOn()                PORTC |=  _BV(OUT_H_HAPTIC)
#define hapticOff()               PORTC &= ~_BV(OUT_H_HAPTIC)

// Buzzer driver
#define buzzerOn()                PORTC |=  _BV(OUT_H_SpeakerBuzzer)
#define buzzerOff()               PORTC &= ~_BV(OUT_H_SpeakerBuzzer)

// Speaker driver
#if defined(AUDIO) && !defined(SIMU)
#define speakerOff()              TCCR4A &= ~_BV(COM4A0)
#define speakerOn()               TCCR4A |=  _BV(COM4A0)
#endif

// Voice driver

//JQ6500
#define JQ6500_BUSY                   (PINJ & _BV(INP_J_JQ_BUSY))
#if defined(VOICE_JQ6500)
extern void InitJQ6500UartTx();
#endif

void boardInit();
//ISR(INT7_vect);
//ISR(INT6_vect)
//{
//
//}
//ISR(INT5_vect)
//{
	//if (!(PINE & ROT_ENC_1_MASK)) {debounceRotEncA(); incRotaryEncoder(0, -1);}
//}
//ISR(INT6_vect)
//{
	//if (!(PINE & ROT_ENC_1_MASK)) {debounceRotEncA(); incRotaryEncoder(0, +1);}
//}

extern uint8_t rotEncADebounce;

#define ROTENCDEBOUNCEVAL _BV(2)
extern uint8_t rotEncADebounce;
extern uint8_t rotEncBDebounce;

#ifdef ROTARY_ENCODERS
#define IF_ROTARY_ENCODERS(x) x,
#else
#define IF_ROTARY_ENCODERS(x)
#endif

// EEPROM driver
#if !defined(SIMU)

#if defined(EXTERNALEEPROM)
#define ADDRESS_EXTERN_EEPROM  (0x50 << 1) //0x50 with all strap closed on ZS042 module (or alone), EEPROM FM24W256, see datasheet
#define EEPROMREADBLOCK(a, b, c)   Ext_eeprom_read_block(a, b, c) //External EEPROM
#else
#define EEPROMREADBLOCK(a, b, c)   eeprom_read_block(a, (const void *)b, c) //Internal EEPROM
#endif


#endif

//SUPIIIK FILE
#if defined (MULTIMODULE)
#define PROTO_HAS_MULTI
#endif
//SUPIIIK FILE



#if defined(SPIMODULES)

//uint8_t USART2_mspi_xfer(uint8_t data);
//void rf_usart_mspi_init();
//#define RF_SPI_INIT             rf_usart_mspi_init
//#define RF_SPI_xfer             USART2_mspi_xfer
//#define OUT_J_CC2500_CS_N       PIN3_bm
//#define OUT_J_CYRF6936_CS_N     PIN4_bm
//#define OUT_J_NRF24L01_CS_N     PIN5_bm
//#define OUT_J_A7105_CS_N        PIN6_bm
//
//#define RF_CS_CC2500_ACTIVE()     PORTJ &= ~(OUT_J_CC2500_CS_N)
//#define RF_CS_CC2500_INACTIVE()   PORTJ |= (OUT_J_CC2500_CS_N)
//#define RF_CS_CYRF6936_ACTIVE()   PORTJ &= ~(OUT_J_CYRF6936_CS_N)
//#define RF_CS_CYRF6936_INACTIVE() PORTJ |= (OUT_J_CYRF6936_CS_N)
//#define RF_CS_NRF24L01_ACTIVE()   PORTJ &= ~(OUT_J_NRF24L01_CS_N)
//#define RF_CS_NRF24L01_INACTIVE() PORTJ |= (OUT_J_NRF24L01_CS_N)
//#define RF_CS_A7105_ACTIVE()      PORTJ &= ~(OUT_J_A7105_CS_N)
//#define RF_CS_A7105_INACTIVE()    PORTJ |= (OUT_J_A7105_CS_N)

// PORTH0 RXD2
// PORTH1 TXD2
// PORTH2 XCK2
//#define OUT_H_RF_MOSI             PIN1_bm
//#define OUT_H_RF_XCK              PIN2_bm
//#define SET_RF_XCK_IS_OUTPUT()    DDRH |= (OUT_H_RF_XCK)
//#define RF_XCK_ON()               PORTH |= (OUT_H_RF_XCK)
//#define RF_XCK_OFF()              PORTH &= ~(OUT_H_RF_XCK)
//#define SET_RF_MOSI_IS_INPUT()    DDRH &= ~(OUT_H_RF_MOSI)
//#define SET_RF_MOSI_IS_OUTPUT()   DDRH |= (OUT_H_RF_MOSI)
//#define IS_RF_MOSI_ON             (PINH & (OUT_H_RF_MOSI))
//#define SUSPEND_RF_SPI()          UCSR2C = 0x00;UCSR2B = 0x00 // Reset and Disable RX and TX Mspi mode
//#define WAKEUP_RF_SPI()           UCSR2C = 0xC0;(UCSR2B = (1 << RXEN2) | (1 << TXEN2)) // Enable RX and TX Mspi mode

#endif // SPIMODULES

#endif /* DX6I_H_ */