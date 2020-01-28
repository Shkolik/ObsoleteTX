/*
 * _Config.h
 *
 * Created: 1/24/2020 9:13:43 PM
 *  Author: Andrew
 */ 


#ifndef _CONFIG_H_
#define _CONFIG_H_

#define F_CPU 8000000UL

#define TIMERS 2
// Stick tolerance varies between transmitters, Higher is better
#define STICK_TOLERANCE 64
#define EXTRA_3POS 0
#define DEFAULT_MODE 1

// board type
#define DX6I
#define MULTIMODULE
#define GUI
#define SPIMODULES
#define FLIGHT_MODES
#define CURVES
#define TENPLATES

#endif /* _CONFIG_H_ */