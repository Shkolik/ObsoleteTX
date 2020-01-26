/*
 * timers.h
 *
 * Created: 1/25/2020 10:09:15 PM
 *  Author: Andrew
 */ 


#ifndef TIMERS_H_
#define TIMERS_H_



#define TMR_OFF      0
#define TMR_RUNNING  1
#define TMR_NEGATIVE 2
#define TMR_STOPPED  3

#define TIMER_MAX     (0xffff/2)

#define TIMER_MIN     (int16_t(-TIMER_MAX-1))

struct TimerState {
	uint16_t cnt;
	uint16_t sum;
	uint8_t  state;
	int16_t  val;
	uint8_t  val_10ms;
};

extern TimerState timersStates[TIMERS];

void timerReset(uint8_t idx);


void saveTimers();
void restoreTimers();

void evalTimers(int16_t throttle, uint8_t tick10ms);


#endif /* TIMERS_H_ */