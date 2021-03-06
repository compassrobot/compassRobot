/*----------------------------------------------------------------------------
 *      
 *----------------------------------------------------------------------------
 *      Name:    slcd.h
 *      Purpose: Microprocessors Laboratory
 *----------------------------------------------------------------------------
 *      
 *---------------------------------------------------------------------------*/

#ifndef slcd_h
#define slcd_h

#include "MKL46Z4.h"   /* Device header */

#define LCD_N_FRONT 8
// Definition of segments, each 7-segments (4 digits) is controled by two pins
// Pin 1 -> (2*digit - 1), Pin 2 -> (2*digit - 2)
//  COM  Pin 1 Pin 2
//   0    D    Dec
//   1    E    C
//   2    G    B
//   3    F    A
#define LCD_S_D 0x11 // seg D
#define LCD_S_E 0x22 // seg E
#define LCD_S_G 0x44 // seg G
#define LCD_S_F 0x88 // seg F
#define LCD_S_DEC 0x11
#define LCD_S_C 0x22
#define LCD_S_B 0x44
#define LCD_S_A 0x88
#define LCD_C 0x00 // clear
//definition of each pin connected to sLCD
#define LCD_FRONT0 37u
#define LCD_FRONT1 17u
#define LCD_FRONT2 7u
#define LCD_FRONT3 8u
#define LCD_FRONT4 53u
#define LCD_FRONT5 38u
#define LCD_FRONT6 10u
#define LCD_FRONT7 11u

const static uint8_t LCD_Front_Pin[LCD_N_FRONT] = {LCD_FRONT0, LCD_FRONT1, LCD_FRONT2, LCD_FRONT3, LCD_FRONT4, LCD_FRONT5, LCD_FRONT6, LCD_FRONT7};

void slcdInitialize(void);
void slcdErr(uint8_t number);
void slcdSet(uint8_t value,uint8_t digit);
void OFFdwukropek(void);
void ONdwukropek(void);
void OFFkropka(uint8_t p);
void ONkropka(uint8_t p);

void slcdDisplay(uint16_t value,uint16_t format);
void slcdClear(void);

#endif
