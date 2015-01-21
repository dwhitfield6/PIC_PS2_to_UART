/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#define USE_OR_MASKS
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

double BatteryVoltage =0;

double ReadVoltage(void)
{
    unsigned int temp;
    double voltage;
    LATC &= ~DivON;//power voltage divider
    temp = InternalADC_Read(2);
    DisableInternalADC();
    LATC |= DivON;//release voltage divider
    //reference is 1.024 and its 10 bits
    //therefore 1.024/1024 = 1000
    voltage = ((double)temp / 1000) * ratio;
    return voltage;
}

unsigned int InternalADC_Read(unsigned char channel)
{
    ADCON0 =0;
    ADCON1 =0;
    ADCON1 |= 0b01010000;//FOSC/16
    FVRCON |= FVREN;
    FVRCON |= 0x01;//buffer is set to 1x
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
void DisableInternalADC(void)
{
    ADCON0 &= ~ADON;
}
