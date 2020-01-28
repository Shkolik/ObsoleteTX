/*
 * widgets.h
 *
 * Created: 1/27/2020 8:02:26 PM
 *  Author: Andrew
 */ 


#ifndef WIDGETS_H_
#define WIDGETS_H_


extern void drawStick(coord_t centrex, int16_t xval, int16_t yval);
extern void menu_lcd_onoff(coord_t x, coord_t y, uint8_t value, LcdFlags attr);

#endif /* WIDGETS_H_ */