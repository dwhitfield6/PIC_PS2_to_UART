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
 *                          Tagged
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

unsigned char Version[] = {"1.2"};

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

extern double BatteryVoltage;
extern unsigned char Alarm;

unsigned int SinLEDtimer =0;
unsigned int pwrLEDtoggle =0;

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

    PS_2_DISABLE_INTERRUPT(CLK);
    //let the host system initialize
    UARTstringWAIT("\r\n");
    delayUS(Word_Spacing);
    UARTstringWAIT("PS/2 Keyboard to RS-232\r\n");
    delayUS(Character_Spacing);
    UARTstringWAIT("Firmware Version: ");
    UARTstringWAIT(Version);
    UARTstringWAIT("\r\n");
    delayUS(Character_Spacing);
    UARTstringWAIT("To Change BAUD hit \"CNT+ALT+DEL\"\r\n");
    delayUS(Word_Spacing);

    //check twice for a disconnecteed keyboard then print
    if(!Keyboard_Connected())
    {
        if(!Keyboard_Connected())
        {
            UARTstringWAIT("No Keyboard connected\r\n");
            //wait here until a keyboard is connected
            while(!Keyboard_Connected())
            {
                BatteryVoltage = ReadVoltage();
                //Check to see if voltage is out of range
                if(BatteryVoltage < VoltageLow || BatteryVoltage > VoltageHigh)
                {
                    LATC ^= pwrLED;
                    pwrLEDtoggle = 0;
                }
                else
                {
                    LATC |= pwrLED;
                }
            }
        }
    }
    UARTstringWAIT("Keyboard Connected\r\n");
    if(!Init_PS_2_Send())
    {
        delayUS(Word_Spacing);
        UARTstringWAIT("Initialization Fail\r\n");
    }
    else
    {
        UARTstringWAIT("Keyboard Pass!\r\n");
    }
    BatteryVoltage = ReadVoltage();
    if(BatteryVoltage < VoltageLow )
    {
        UARTstringWAIT("Voltage too Low!\r\n");
    }
    else if (BatteryVoltage > VoltageHigh)
    {
        UARTstringWAIT("Voltage too High!\r\n");
    }
    PS_2_ENABLE_INTERRUPT(CLK);

    while(1)
    {
        BatteryVoltage = ReadVoltage();
        if(BatteryVoltage < VoltageLow || BatteryVoltage > VoltageHigh)
        {
            pwrLEDtoggle++;
            if(pwrLEDtoggle > pwrLEDtime)
            {
                LATC ^= pwrLED;
                pwrLEDtoggle = 0;
            }
        }
        else
        {
            LATC |= pwrLED;
        }
        if(SinLEDtimer >= SinLEDTimeOut)
        {
            LATC &= ~SinLED;
        }
        else
        {
            SinLEDtimer++;
        }
        PS_2_Update();
    }
}

