/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 01/09/15     1.0C        Written.
 * 01/20/15     1.0F        Commented out Timer2 interrupt since we no longer
 *                            use this to flash the power LED.
 * 01/21/15     1.1         Fixed error in calculating the parity bit for
 *                            initialization.
 * 01/23/15     1.2         Fixed Baud rate sting logic to save memory.
 *                          Added "keyboard" before every baud rate change.
/******************************************************************************/

/******************************************************************************/
/* Contains Functions for PIC initialization
 *
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
#include <stdio.h>         /* For sprintf definition */

#endif

#include "user.h"
#include "UART.h"
#include "FLASH.h"
#include "MISC.h"
#include "Timer.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

unsigned char Alarm =0;

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitApp
 *
 * The function initializes the application. It sets the pins and pull-ups.
/******************************************************************************/
void InitApp(void)
{
    LATC =0;
    LATA =0;
    TXTRIS     = OUTPUT;
    pwrLEDTRIS = OUTPUT;
    SinLEDTRIS = OUTPUT;
    KeyLEDTRIS = OUTPUT;
    BatINTRIS  = INPUT;
    DivONTRIS  = OUTPUT;  

    //turn on power LED and initialize the pins
    LATC |= pwrLED;
    PPSLOCK =0;//unlock PPS
    RC2PPS = 0b00010100;//RC2 is TX/CK
    CKPPS = 0b00010010;//RC2 is TX
    RXPPS = 0b00010101;//RC5 is RX
    PPSLOCK =1; //lock PPS

    ANSELA &= ~0xFF;//set ALL PortA to digital
    ANSELA |= 0b00000100;//set RA2 to analog
    ANSELC &= ~0xFF;//set ALL PortC to digital
    OPTION_REGbits.nWPUEN = 0;//pull-ups enabled
    WPUC = 0b00100000;//enable RC5 (RX) pull-up
    WPUA = 0;
}

/******************************************************************************/
/* Init_System
 *
 * The function initializes the system by setting up the initial starting baud
 *   and prints the baud rate over the baud.
/******************************************************************************/
void Init_System (void)
{
    unsigned long temp =0;
    unsigned long Baud =0;
    unsigned char Parity =0;
    unsigned char buf[60];

    temp = ReadBaud(FLASH_ADDRESS_ROW,0);
    Baud   = temp & 0x000FFFFF;
    Parity = (unsigned char)((temp & 0x00F00000) >> 20);
    if(Baud <2400 || Baud > 115200)
    {
        //default is 9600 with no parity bit
        Baud = 9600;
        Parity = 0;
    }
    InitUART(Baud, Parity);
    UARTstringWAIT("\r\n");
    delayUS(Word_Spacing);
    sprintf(buf,"KeyBoard Baud is %lu",Baud);
    UARTstringWAIT(buf);
    if(Parity)
    {
        switch (Parity)
        {
            case 1:
                UARTstringWAIT(" with Odd parity bit\r\n");//Odd parity
                break;
            case 2:
                UARTstringWAIT(" with Even parity bit\r\n");//Even parity
                break;
            case 3:
                UARTstringWAIT(" with Mark bit\r\n");//mark
                break;
            default:
                UARTstringWAIT(" with Space bit\r\n");//Space
                break;
        }
    }
    else
    {
        UARTstringWAIT(" with no parity bit\r\n");
    }
    delayUS(Word_Spacing);
    //Timer2Init(0xFF);
    INTCON |= iocie;// int on change
    INTCONbits.GIE = 1;
}