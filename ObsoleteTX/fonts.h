/*
 * fonts.h
 *
 * Created: 1/27/2020 8:34:50 PM
 *  Author: Andrew
 */ 


#ifndef FONTS_H_
#define FONTS_H_


extern const uint8_t font_5x7_in_sram[];
extern const uint8_t font_10x14_in_sram[];

#if defined(BOLD_FONT)
#define BOLD_SPECIFIC_FONT
extern const uint8_t font_5x7_B_in_flash[];
#endif



#endif /* FONTS_H_ */