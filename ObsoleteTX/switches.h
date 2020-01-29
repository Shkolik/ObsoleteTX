/*
 * switches.h
 *
 * Created: 1/26/2020 8:56:42 PM
 *  Author: Andrew
 */ 


#ifndef SWITCHES_H_
#define SWITCHES_H_

typedef struct {
	uint8_t state;
	uint8_t last;
} __attribute__((__packed__)) ls_sticky_struct;



#endif /* SWITCHES_H_ */