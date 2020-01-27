/*
 * eeprom_common.h
 *
 * Created: 1/25/2020 10:05:49 PM
 *  Author: Andrew
 */ 


#ifndef EEPROM_COMMON_H_
#define EEPROM_COMMON_H_


#define WRITE_DELAY_10MS 200

#define TIME_TO_WRITE() (s_eeDirtyMsk && (uint16_t)(getTmr10ms() - s_eeDirtyTime10ms) >= (uint16_t)WRITE_DELAY_10MS)

extern uint8_t   s_eeDirtyMsk;
extern uint16_t s_eeDirtyTime10ms;

extern void eeDirty(uint8_t msk);
extern void eeCheck(bool immediately);
extern void eeReadAll();
extern bool eeModelExists(uint8_t id);
extern void eeLoadModel(uint8_t id);
extern uint8_t eeConvert();
extern void eeErase(bool warn);
extern uint8_t eeFindEmptyModel(uint8_t id, bool down);
extern void selectModel(uint8_t sub);


#endif /* EEPROM_COMMON_H_ */