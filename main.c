/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 01/09/15     1.0C_DW0a   Changed frequency to 32MHz.
 *                          Commented out code to allow it to fit in 16F1704.
 * 01/15/15     1.0D_DW0a   Changed MCU to the PIC16f1705 which has 4k more
 *                            memory.
 *                          Added code to change/add a parity bit.
 *                          Added UART receive functionality.
 *                          Enabled pull-up of rx pin.
 *                          Detect break character functionality.
 * 01/15/15     1.0E_DW0a   Added ADC to read value of battery.
 * 01/20/15     1.0F_DW0a   Bug fixes.
 * 01/20/15     1.1         Changed name from 1.0F_DW0a.
 *                          Tagged.
 * 01/20/15     1.1_DW0a
 *                          Add code for Version number.
 *                          Change BAUD flash address to 0x1800.
 *                          Protect code with code protect bit enabled in
 *                            configuration bits.
 *                          Fixed bugs with shift key and Caps lock key.
 * 01/26/15     1.2         Changed name from 1.1_DW0a.
 *                          Add log comments.
 *                          Add function comments.
 *                          Add function prototype and define comments.
 *                          Add 'KeyBoard' to Baud comments.
 *                          Check voltage when keyboard is not connected.
 *                          Add macro to allow the voltage equation to be
 *                            compensated for reference change.
 *                          Check twice for disconnected keyboard.
 *                          Changed system from short break to long break.
 *                          Create macro for the distingtion between
 *                            products.
 *                          Create macro for the adding a new line with each
 *                            carriage return.
 *                          Added macro for arduino shield system.
 *                          Added code to allow for receiving when the keyboard
 *                            is not connected.
 *                          Add function READ_CONFIG_PIN().
 *                          Add code for 'Config' Pin on RS232 system.
 *                          Use device macro to define the use of pin RC0.
 *                          Tagged
 * 03/11/15     1.3_DW0a     Added wait to let previous character send before
 *                            before refilling the buffer.
 *                          Delete the check keyboard routine.
 *                          Add code for print screen button rejection.
 *                          Momentarily turn on all keyboard leds at power on.
 *                          Make the pause/break key send a break.
 *                          Fix functionality for control key and alt key.
 *                          Define out the config code when product is an
 *                            Arduino.
 *                          Decreased timeout for keypress to increase
 *                            throughput.
 *                          Separated long break when 'end' is pressed and short
 *                            and long break when 'pause' is pressed.
 *                          Add line feed when f3 is pressed with a rs232
 *                            system.
 *                          Fixed UART recieve overrun error.
 *                          Fixed UART standalone build.
 *                          Dropped minimum voltage to 5.25 V.
/******************************************************************************/

/******************************************************************************/
/* Contains main function.
 *
/******************************************************************************/

/******************************************************************************/
/* Pick Which System!!!
 *
 * Go to user.h and define if the system is the RS232 model or TTL model
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#define USE_OR_MASKS

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "MISC.h"
#include "UART.h"
#include "PS_2.h"
#include "FLASH.h"
#include "ADC.h"

/******************************************************************************/
/* Version number                                                             */
/******************************************************************************/

const unsigned char Version[] = {"1.3_DW0a"};

/******************************************************************************/
/* Defines                                           */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned int SinLEDtimer =0;
unsigned int pwrLEDtoggle =0;
unsigned char Alarm;
double BatteryVoltage = 0.0;

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{
    unsigned char i=0;

    ConfigureOscillator();
    InitApp();
    Init_System();
    PS_2_INIT();

    //blink LED
    for(i =0;i<10;i++)
    {
       LATC |= KeyLED;
       delayUS(10000);
       LATC &= ~KeyLED;
       delayUS(10000);
    }
        
    BatteryVoltage = ReadVoltage();
    if(BatteryVoltage < VoltageLow )
    {
        UARTstringWAIT(VoltageMSG);
        UARTstringWAIT("Low!\r\n");
    }
    else if (BatteryVoltage > VoltageHigh)
    {
        UARTstringWAIT(VoltageMSG);
        UARTstringWAIT("High!\r\n");
    }

    PS_2_ENABLE_INTERRUPT(CLK);
    #ifndef ARDUINO
    PIE1bits.RCIE = 1;//rx interrupt enable
    #endif

    while(1)
    {
        BatteryVoltage = ReadVoltage();
        delayUS(20);
        if(BatteryVoltage < VoltageLow || BatteryVoltage > VoltageHigh)
        {
            Alarm = 1;
        }
        else
        {
            Alarm = 0;
            LATC |= pwrLED;
        }
        if(Alarm)
        {
            pwrLEDtoggle++;
            if(pwrLEDtoggle == pwrLEDFlashRate)
            {
               pwrLEDtoggle = 0;
               LATC ^= pwrLED;
            }            
        }

        if(SinLEDtimer < SinLEDTimeOut)
        {
            SinLEDtimer++;
        }
        else
        {
            LATC &= ~SinLED;
        }                
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
