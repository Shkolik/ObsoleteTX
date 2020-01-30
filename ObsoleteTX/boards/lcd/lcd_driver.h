/*
 * lcd_driver.h
 *
 * Created: 1/29/2020 8:27:22 PM
 *  Author: andrew.shkolik
 */ 


#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include "../../ObsoleteTX.h"

#if defined(LCD_KS108)
#include "lcd_ks108_driver.h"
#elif defined(LCD_ST7920)
#include "lcd_st7920_driver.h"
#elif defined(LCD_SSD1306)
#include "lcd_ssd1306_driver.h"
#elif defined(LCD_SH1106)
#include "lcd_sh1106_driver.h"
#elif defined(LCD_NT7538SPI)
#include "lcd_NT7538_SPI_driver.h"
#else
#include "lcd_default_driver.h"
#endif

#endif /* LCD_DRIVER_H_ */