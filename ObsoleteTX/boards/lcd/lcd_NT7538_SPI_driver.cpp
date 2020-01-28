///*
 //*
 //* Created: 1/25/2020 1:27:56 AM
 //*  Author: Andrew
 //*/ 
//
//
//
//#include "../../ObsoleteTX.h"
//
//#include "spi.h"
//
//#define ST7565R_CMD_RESET                       0xE2
//#define ST7565R_CMD_DISPLAY_OFF                 0xAE
//#define ST7565R_CMD_DISPLAY_ALL_POINTS_OFF      0xA4
//#define ST7565R_CMD_REVERSE_SCAN_DIRECTION      0xC8
//#define ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET  0x81
//#define ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_5    0x25
//#define ST7565R_CMD_POWER_CTRL_ALL_ON           0x2F
//#define ST7565R_CMD_DISPLAY_ON                  0xAF
//#define ST7565R_CMD_START_LINE_SET( line )             (0x40 | (line))
//#define ST7565R_CMD_PAGE_ADDRESS_SET( page )           (0xB0 | (page))
//#define ST7565R_CMD_COLUMN_ADDRESS_SET_MSB( column )   (0x10 | (column))
//#define ST7565R_CMD_COLUMN_ADDRESS_SET_LSB( column )   (0x00 | (column))
//
//#define A0_on                    PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_A0)
//#define A0_off                   PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_A0)
//
//#define NUMITERATIONFULLREFRESH  2
//
//
//void lcdSendCtl(uint8_t c)
//{
  //A0_off;
  ////LCD_CS_P_ACTIVE;
  //master_spi_xfer(c); // Master SPI.
  ////LCD_CS_P_INACTIVE;
  //A0_on;
//}
//
//
//void lcdSetRefVolt(uint8_t val)
//{
  //lcdSendCtl(ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET); // Range = 0x09 to 0x19 (-8 to +8 on Evo setup screen)
  //lcdSendCtl(val>>1);
//}
//
//void lcdInit()
//{
  ////PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_RES);  //LCD reset
  ////_delay_ms(200);
  //PORTC_LCD_CTRL |= _BV(OUT_C_LCD_RES);  //LCD normal operation
  //enable_spi_master_mode();
  ////SPI_4M();
  ////LCD_CS_P_INACTIVE;
  //lcdSendCtl(ST7565R_CMD_RESET);
  //_delay_us(300);
  //lcdSendCtl(0x2C);// Power controller set, booster cct on, V reg off, V follower off
  //_delay_us(300);
  //lcdSendCtl(0x2E);// Power controller set, booster cct on, V reg on, V follower off
  //_delay_us(300);
  //lcdSendCtl(ST7565R_CMD_POWER_CTRL_ALL_ON);// Power controller set, booster cct on, V reg on, V follower on
  //_delay_us(300);
  //lcdSendCtl(ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_5);
//
  //lcdSendCtl(ST7565R_CMD_DISPLAY_ON);
  //// Turn display on in lcdRefresh() ... prevents displaying garbage at startup, which seems to happen more when sdcards are attached.
//
  //lcdSendCtl(ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET);// Electronic volume mode set ? Range = 0x09 to 0x19 (-8 to +8 on Evo)
  //lcdSendCtl(0x09);// See above ... 0 = 0x11
  //lcdSendCtl(ST7565R_CMD_REVERSE_SCAN_DIRECTION);// Common output mode select
  //lcdSendCtl(ST7565R_CMD_DISPLAY_ALL_POINTS_OFF);// All points off (normal)
//
  //g_eeGeneral.contrast = 0x09;
//}
//
//
//void lcdRefreshFast()
//{
//#if defined(SHOWDURATION)
  //lcdDrawNumberAttUnit(16*FW, 1, DURATION_MS_PREC2(DurationValue), PREC2);
//#endif
  //SHOWDURATIONLCD1
//
  //static uint8_t offset = 0;
  //uint8_t * p = displayBuf;
  //if(offset) p += 512;
//
  //lcdSendCtl(ST7565R_CMD_DISPLAY_ON);
//
  //for(uint8_t page=0; page<4; page++) {
    //lcdSendCtl(ST7565R_CMD_COLUMN_ADDRESS_SET_LSB(0)); // Set LS nibble column RAM address 0
    //lcdSendCtl(ST7565R_CMD_COLUMN_ADDRESS_SET_MSB(0)); // Set MS nibble column RAM address 0
    //lcdSendCtl(ST7565R_CMD_PAGE_ADDRESS_SET(page + offset));
//
    ////LCD_CS_P_ACTIVE;
    //master_spi_xfer(0x00); // Pad out the four unused columns.
    //master_spi_xfer(0x00);
      //for(unsigned char x=LCD_W; x>0; --x) {
      //master_spi_xfer( *p++ );
      //}
    //master_spi_xfer(0x00);
    //master_spi_xfer(0x00);
    ////LCD_CS_P_INACTIVE;
  //}
  //offset ^=4;
  //SHOWDURATIONLCD2
//}
//
//
//void lcdRefresh()
//{
  //for(uint8_t i=NUMITERATIONFULLREFRESH; i>0; i--) {
    //lcdRefreshFast();
  //}
//}
