///*
 //*
 //* Created: 1/25/2020 1:27:56 AM
 //*  Author: Andrew
 //*/ 
//
//
//#define NUMITERATIONFULLREFRESH  4
//
//#define DISPLAY_SET_COLUMN       0x40
//#define DISPLAY_SET_PAGE         0xB8
//#define DISPLAY_SET_START        0XC0
//#define DISPLAY_ON_CMD           0x3F //or 3E
//#define CS1_on                   PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_CS1)
//#define CS1_off                  PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_CS1)
//#define CS2_on                   PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_CS2)
//#define CS2_off                  PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_CS2)
//#define A0_on                    PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_A0)
//#define A0_off                   PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_A0)
//#define E_on                     PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_E)
//#define E_off                    PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_E)
//
//void lcdPulseEnable()
//{
  //E_on;
  //_delay_us(9);
  //E_off;
//}
//
//void lcdSendCtl(uint8_t val)
//{
  //PORTA_LCD_DAT = val;
  //A0_off;
  //lcdPulseEnable();
  //A0_on;
//}
//
//static void LcdInitCommand()
//{
  //lcdSendCtl(DISPLAY_ON_CMD);
  //lcdSendCtl(DISPLAY_SET_START);
//}
//
//void lcdInit()
//{
  //PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_RES);  //LCD reset
  //_delay_us(20);
  //PORTC_LCD_CTRL |= _BV(OUT_C_LCD_RES);  //LCD normal operation
  //CS1_on;                                //Init KS108 who need hight level on CS pin
  //CS2_on;
  //LcdInitCommand();                      //Init the two KS in one time
  //CS1_off;                               //Init KS108 who need low level on CS pin
  //CS2_off;
  //LcdInitCommand();
//}
//
//void lcdSetRefVolt(uint8_t val)
//{
//}
//
//void lcdRefreshFast()
//{
//SHOWDURATIONLCD1
//#if defined(SHOWDURATION)
  //lcdDrawNumberNAtt(16*FW, 1, DURATION_MS_PREC2(DurationValue), PREC2);
//#endif
  //static uint8_t step = 0; // toggle left/right/high/low sector of lcd writing
  //uint8_t *p;
  //if (step>3) step = 0;
  //if (step<2) {
    //CS2_off;  // Right
    //CS1_on;
    //p = displayBuf;
  //} else {
    //CS1_off;  // Left
    //CS2_on;
    //p = displayBuf + 64;
  //}
  //uint8_t offset = (step & 0x01);
  //uint8_t start = 4 * offset;
  //uint8_t stop = 4 + start;
  //p += offset*512; //add half screen offset
//
//
  //for (uint8_t page=start; page < stop; page++) {
    //lcdSendCtl(DISPLAY_SET_COLUMN); // Column addr 0
    //lcdSendCtl( page | DISPLAY_SET_PAGE); //Page addr
    //A0_on;
    //for (coord_t x=64; x>0; --x) {
      //PORTA_LCD_DAT = *p++;
      //lcdPulseEnable();
    //}
    //p += 64;
  //}
  //++step;
  //A0_off;
//SHOWDURATIONLCD2
//}
//
//void lcdRefresh()
//{
  //for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++) {
    //lcdRefreshFast();
  //}
//}
