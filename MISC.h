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

#include "system.h"

/******************************************************************************/
/* Delay constant
 *
 * This parameter is set to calibrate the delayUS() function. Parameter found
 *   from guess and check.
/******************************************************************************/

#define delayConst 6
//inverse relationship

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define SYS_FREQ_US 32
#define Even 2
#define Odd 3
#define PASS 1
#define FAIL 0

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/

void delayUS(long US);
void cleanBuffer(unsigned char* data, int count);
unsigned char CheckSum_byte(unsigned int This, unsigned char Odd_Even);
unsigned char Reverse_Byte(unsigned char This);
unsigned char ISNUMBER(unsigned char ascii);
unsigned char BufferShiftBack(unsigned char* buffer, unsigned char shift, unsigned char size);
