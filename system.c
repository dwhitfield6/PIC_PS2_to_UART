/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 01/21/15     1.2         Added log.
/******************************************************************************/

/******************************************************************************/
/* Contains system functions.
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* ConfigureOscillator
 *
 * The function waits for the high frequency occilator to be working and stable.
/******************************************************************************/
void ConfigureOscillator(void)
{
    OSCCON= 0b11110000;//32MHz
    while(!(OSCSTAT & HFIOFS));//Wait for High frequency inernal oscillator to be ready
    while(!(OSCSTAT & PllR));//Wait for PLL to be ready
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/