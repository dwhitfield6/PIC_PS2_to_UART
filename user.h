/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 01/21/15     1.2         Created log.
 *                          Create macro for the distingtion between
 *                            products.
 *                          Create macro for the adding a new line with each
 *                            cariage return.
 * 03/11/15     1.3_DW0a    Dropped minimum voltage to 5.25 V.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef USER_H
#define	USER_H

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/******************************************************************************/
/* Pick Which System!!!
 * 
 * Choose if the system is the RS232 model, TTL model (UART standalone),
 *  or Arduino Module.
 *
 * For an RS-232 system (includes rs 232 drivers and is powered by power brick.
 *  Uncomment the RS232 macro below.
 *
 * For the TTL version that goes between the main sustem and the USB to TTL
 *  UART converter, Comment both below.
 *
 * For the Arduino version, we dont need any UART RX. And the system is
 *  powered by 5 volts. comment RS232 and uncomment ARDUINO
 *
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#define RS232
//#define ARDUINO

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/******************************************************************************/
/* Pick System Config!!!
 *
 * Choose if the system inserts a new line after a return. This is needed by
 *   some software but causes 2 new line in others.
 *
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef RS232
#define ADDLINE
#endif

/******************************************************************************/
/* PCB board
 *
 * This code is used with boards:
 * 1. PS2 to UART 6pin_revA
 * 2. PS2 to UART 6pin_revB
 * 3. Arduino PS_2_shield
 * 4. PS2 to UART 6pin_revC
 * 5. PS2 to UART RS232_revA
 * 6. PS2 to UART RS232_revB
 * 7. PS2 to UART RS232_revC
/******************************************************************************/

/******************************************************************************/
/* Important parameters.
 *
 * SinLEDTimeOut determines the amount of cycles that the Serial in LED stays
 *   on after a character is receive.
 *
 * pwrLEDtime modifies the blink rate of the power led when the voltage is
 *   higher than VoltageHigh or lower than VoltageLow.
 *
 * BlinkDelay is not used in this code. It is only used when the timer 2
 *   interrupt is used to blink the pwrLED.
 *
 * VoltageLow is the low voltage threshold. If the supply voltage is lower than
 *   this value, the power led blinks.
 *
 * VoltageHigh is the high voltage threshold. If the supply voltage is higher
 *   than this value, the power led blinks.
 *
 * Note:When the system is driven by the 7805 LDO, the lowest input voltage is
 *   ~5.8 volts.
 *
/******************************************************************************/

#define SinLEDTimeOut       100
#define pwrLEDFlashRate     100
#define BlinkDelay    4

#ifdef RS232
#define VoltageLow 6.55
#define VoltageHigh 30.0
#else
#define VoltageLow 4.25
#define VoltageHigh 5.35
#endif
/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define ON 1
#define OFF 0
#define OUTPUT 0
#define INPUT 1
#define gie 0x80
#define iocie 0x08
#define TXTRIS	TRISCbits.TRISC2
#define TX 0b00000100 //RC2
#define RXTRIS	TRISCbits.TRISC5
#define RX 0b00100000 //RC5
#define pwrLEDTRIS	TRISCbits.TRISC1
#define pwrLED 0b00000010 //RC1
#define SinLEDTRIS	TRISCbits.TRISC3
#define SinLED 0b00001000 //RC3
#define KeyLEDTRIS	TRISCbits.TRISC4
#define KeyLED 0b00010000 //RC4
//BatIN is the battery voltage cut in half by the voltage divider
#define BatINTRIS	TRISAbits.TRISA2
#define BatIN 0b00000100 //RA2 OR an2
//DivON turns on the voltage divider to take a reading
#ifdef RS232
#define configTRIS	TRISCbits.TRISC0
#define Config 0b00000001 //RC0
#else
#define DivONTRIS	TRISCbits.TRISC0
#define DivON 0b00000001 //RC0
#endif



/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/

void InitApp(void);        
void Init_System (void);


#endif	/* USER_H */