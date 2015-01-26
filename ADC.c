/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 01/26/15     1.2         Created log.
 *                          Add macro to allow the voltage equation to be
 *                            compensatied for reference change.
 *                          Added function ShutDown_ADC to reset ADC module.
/******************************************************************************/

/******************************************************************************/
/* Contains functions that control the internal ADC.
 *
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

#include "ADC.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "UART.h"          /* User funct/params, such as InitApp */
#include "MISC.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define USE_OR_MASKS

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

double BatteryVoltage =0;

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* ReadVoltage
 *
 * The function returns the voltage that powers the unit. There is a voltage
 * divider that is turned on (DivON) and the divided voltage is read.
/******************************************************************************/
double ReadVoltage(void)
{
    unsigned int temp;
    double voltage;
    LATC &= ~DivON;//power voltage divider
    temp = InternalADC_Read(2);
    DisableInternalADC();
    LATC |= DivON;//release voltage divider
    //reference is 1.024 * reference multiplier and its 10 bits
    //therefore 1.024*Ref_Multiplier/1024 = 1000
    //the bottom 2 bits are the Ref_Multiplier
    if((FVRCON & 0x03) == 0x03)
    {
        //reference is 4 times higher
        temp <<= 2;
    }
    else if((FVRCON & 0x03) == 0x02)
    {
        //reference is 2 times higher
        temp <<= 1;
    }
    voltage = ((double)temp / 1000) * ratio;
    return voltage;
}

/******************************************************************************/
/* InternalADC_Read
 *
 * The function returns the digital adc counts of the channel.
/******************************************************************************/
unsigned int InternalADC_Read(unsigned char channel)
{
    ADCON0 =0;
    ADCON1 =0;
    ADCON1 |= 0b01010000;//FOSC/16
    FVRCON |= FVREN;
    FVRCON |= Ref_Multiplier;//buffer is set to 4x
    while(!(FVRCON & FVRrdy));//wait for reference to be ready
    ADCON0 |= (channel << 2);//input channel
    ADCON1 |= 0b10000011; //right justified and VREF is FVR
    ADCON0 |= ADON;
    ADC_INT_DISABLE();
    delayUS(5);
    ADCON0 |= ADC_GO; //GO
    while(ADCON0 & ADC_GO);
    return ((ADRESH << 8) + ADRESL);
}

/******************************************************************************/
/* DisableInternalADC
 *
 * The function turns off the internal ADC module.
/******************************************************************************/
void DisableInternalADC(void)
{
    ADCON0 &= ~ADON;
}

/******************************************************************************/
/* ShutDown_ADC
 *
 * The function shutsdown the ADC module and sets all registers to their
 *   preinitialized values.
/******************************************************************************/
void ShutDown_ADC(void)
{
    FVRCON =0;
    ADCON0 =0;
    ADCON1 =0;
    ADCON2 =0;
}