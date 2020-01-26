/*
 * eeprom_common.h
 *
 * Created: 1/25/2020 10:05:49 PM
 *  Author: Andrew
 */ 


#ifndef EEPROM_COMMON_H_
#define EEPROM_COMMON_H_


#define WRITE_DELAY_10MS 200

#define TIME_TO_WRITE() (s_eeDirtyMsk && (tmr10ms_t)(getTmr10ms() - s_eeDirtyTime10ms) >= (tmr10ms_t)WRITE_DELAY_10MS)

extern uint8_t   s_eeDirtyMsk;
extern uint16_t s_eeDirtyTime10ms;

void eeDirty(uint8_t msk);
void eeCheck(bool immediately);
void eeReadAll();
bool eeModelExists(uint8_t id);
void eeLoadModel(uint8_t id);
uint8_t eeConvert();
void eeErase(bool warn);
uint8_t eeFindEmptyModel(uint8_t id, bool down);
void selectModel(uint8_t sub);


#endif /* EEPROM_COMMON_H_ */