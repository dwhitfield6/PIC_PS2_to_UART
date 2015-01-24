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

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/

void delayUS(long US);
void cleanBuffer(unsigned char* data, int count);
void BufferCopy(unsigned char* from, unsigned char* to, char count, char shift);
unsigned char StringMatch(unsigned char* This, const unsigned char* That);
unsigned char StringContains(unsigned char* This, const unsigned char* That);
unsigned char StartsWith(unsigned char* This, const unsigned char* That);
int GetEnteredNumber(unsigned char* This);
void RemoveSpaces(unsigned char* This);
void lowercase(unsigned char* Input);
unsigned char StringAddEqual(unsigned char* Input);
unsigned char CheckSum_byte(unsigned int This, unsigned char Odd_Even);
unsigned char Reverse_Byte(unsigned char This);
unsigned char ISNUMBER(unsigned char ascii);
