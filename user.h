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
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
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
 * Choose if the system is the RS232 model or TTL model.
 *
 * For an RS-232 system (includes rs 232 drivers and is powered by ower brick;
 * Uncomment the code below.
 *
 * For the TTL version that goes between the main sustem and the USB to TTL
 *  UART converter, Commetn below.
 *
 * For the Arduino version, we dont need any UART RX. And the system is
 *  powered by 5 volts.
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
 * /******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

//#define ADDLINE

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

#define SinLEDTimeOut 100
#define pwrLEDtime    1000
#define BlinkDelay    4

#ifdef RS232
#define VoltageLow 5.85
#define VoltageHigh 20.0
#else
#define VoltageLow 4.65
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
#define DivONTRIS	TRISCbits.TRISC0
#define DivON 0b00000001 //RC0

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/

void InitApp(void);        
void Init_System (void);
