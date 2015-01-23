/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 01/21/15     1.2         Created log.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* Ratio of voltage divider
 *
 * Set this to the ratio of R2 to R1. The voltage divider = ((R1 + R2) / R1)
/******************************************************************************/
#define ratio 5.7

/* Below is not used due to speed*/

/*
 * #define R1 1000
 * #define R2 4700
 * #define ratio ((R1 + R2) / R1)
 */

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define ADC_INT_ENABLE()     (PIR1bits.ADIF=0,INTCONbits.PEIE=1,PIE1bits.ADIE=1)
#define ADC_INT_DISABLE()    (PIE1bits.ADIE=0)
#define ADC_GO  0b00000010
#define ADON    0b00000001
#define FVREN   0b10000000
#define FVRrdy  0b01000000

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/

double ReadVoltage(void);
void DisableInternalADC(void);
unsigned int InternalADC_Read(unsigned char channel);
