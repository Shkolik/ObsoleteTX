/*
 * fonts.cpp
 *
 * Created: 1/27/2020 7:54:54 PM
 *  Author: Andrew
 */ 
#include "../ObsoleteTX.h"

#if defined(FONTSTD)

const uint8_t font_5x7_in_sram[] = {
	#if defined (LCDROT180)
	#include "../fonts/std/font_05x07_avr.lbmi"
	#if defined(TRANSLATIONS_DE)
	#include "../fonts/std/font_de_05x07.lbmi"
	#elif defined(TRANSLATIONS_CZ)
	#include "../fonts/std/font_cz_05x07.lbmi"
	#elif defined(TRANSLATIONS_ES)
	#include "../fonts/std/font_es_05x07.lbmi"
	#elif defined(TRANSLATIONS_FI)
	#include "../fonts/std/font_fi_05x07.lbmi"
	#elif defined(TRANSLATIONS_FR)
	#include "../fonts/std/font_fr_05x07.lbmi"
	#elif defined(TRANSLATIONS_IT)
	#include "../fonts/std/font_it_05x07.lbmi"
	#elif defined(TRANSLATIONS_PL)
	#include "../fonts/std/font_pl_05x07.lbmi"
	#elif defined(TRANSLATIONS_PT)
	#include "../fonts/std/font_pt_05x07.lbmi"
	#elif defined(TRANSLATIONS_SE)
	#include "../fonts/std/font_se_05x07.lbmi"
	#endif
	#else
	#include "../fonts/std/font_05x07_avr.lbm"
	#if defined(TRANSLATIONS_DE)
	#include "../fonts/std/font_de_05x07.lbm"
	#elif defined(TRANSLATIONS_CZ)
	#include "../fonts/std/font_cz_05x07.lbm"
	#elif defined(TRANSLATIONS_ES)
	#include "../fonts/std/font_es_05x07.lbm"
	#elif defined(TRANSLATIONS_FI)
	#include "../fonts/std/font_fi_05x07.lbm"
	#elif defined(TRANSLATIONS_FR)
	#include "../fonts/std/font_fr_05x07.lbm"
	#elif defined(TRANSLATIONS_IT)
	#include "../fonts/std/font_it_05x07.lbm"
	#elif defined(TRANSLATIONS_PL)
	#include "../fonts/std/font_pl_05x07.lbm"
	#elif defined(TRANSLATIONS_PT)
	#include "../fonts/std/font_pt_05x07.lbm"
	#elif defined(TRANSLATIONS_SE)
	#include "../fonts/std/font_se_05x07.lbm"
	#endif
	#endif
};

#if defined(BOLD_SPECIFIC_FONT)
const pm_uchar font_5x7_B_in_flash[] PROGMEM = {
	#if defined (LCDROT180)
	#include "../fonts/std/font_05x07_B_compressed.lbmi"
	#else
	#include "../fonts/std/font_05x07_B_compressed.lbm"
	#endif
};
#endif

const uint8_t font_10x14_in_sram[] = {
	#if defined (LCDROT180)
	#include "../fonts/std/font_10x14_compressed.lbmi"
	#else
	#include "../fonts/std/font_10x14_compressed.lbm"
	#endif
};

#else //SQT5

const uint8_t font_5x7_in_sram[] = {
	#if defined (LCDROT180)
	#include "../fonts/sqt5/font_05x07_avr.lbmi"
	#if defined(TRANSLATIONS_DE)
	#include "../fonts/sqt5/font_de_05x07.lbmi"
	#elif defined(TRANSLATIONS_CZ)
	#include "../fonts/sqt5/font_cz_05x07.lbmi"
	#elif defined(TRANSLATIONS_ES)
	#include "../fonts/sqt5/font_es_05x07.lbmi"
	#elif defined(TRANSLATIONS_FI)
	#include "../fonts/sqt5/font_fi_05x07.lbmi"
	#elif defined(TRANSLATIONS_FR)
	#include "../fonts/sqt5/font_fr_05x07.lbmi"
	#elif defined(TRANSLATIONS_IT)
	#include "../fonts/sqt5/font_it_05x07.lbmi"
	#elif defined(TRANSLATIONS_PL)
	#include "../fonts/sqt5/font_pl_05x07.lbmi"
	#elif defined(TRANSLATIONS_PT)
	#include "../fonts/sqt5/font_pt_05x07.lbmi"
	#elif defined(TRANSLATIONS_SE)
	#include "../fonts/sqt5/font_se_05x07.lbmi"
	#endif
	#else
	#include "../fonts/sqt5/font_05x07_avr.lbm"
	#if defined(TRANSLATIONS_DE)
	#include "../fonts/sqt5/font_de_05x07.lbm"
	#elif defined(TRANSLATIONS_CZ)
	#include "../fonts/sqt5/font_cz_05x07.lbm"
	#elif defined(TRANSLATIONS_ES)
	#include "../fonts/sqt5/font_es_05x07.lbm"
	#elif defined(TRANSLATIONS_FI)
	#include "../fonts/sqt5/font_fi_05x07.lbm"
	#elif defined(TRANSLATIONS_FR)
	#include "../fonts/sqt5/font_fr_05x07.lbm"
	#elif defined(TRANSLATIONS_IT)
	#include "../fonts/sqt5/font_it_05x07.lbm"
	#elif defined(TRANSLATIONS_PL)
	#include "../fonts/sqt5/font_pl_05x07.lbm"
	#elif defined(TRANSLATIONS_PT)
	#include "../fonts/sqt5/font_pt_05x07.lbm"
	#elif defined(TRANSLATIONS_SE)
	#include "../fonts/sqt5/font_se_05x07.lbm"
	#endif
	#endif
};

#if defined(BOLD_SPECIFIC_FONT)
const pm_uchar font_5x7_B_in_flash[] PROGMEM = {
	#if defined (LCDROT180)
	#include "../fonts/sqt5/font_05x07_B_compressed.lbmi"
	#else
	#include "../fonts/sqt5/font_05x07_B_compressed.lbm"
	#endif
};
#endif

const uint8_t font_10x14_in_sram[] = {
	#if defined (LCDROT180)
	#include "../fonts/sqt5/font_10x14_compressed.lbmi"
	#else
	#include "../fonts/sqt5/font_10x14_compressed.lbm"
	#endif
};

#endif