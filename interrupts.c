/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 01/21/15     1.2         Created log.
 *                          Changed send_break to a timed send break for
 *                            compatibility reasons.
/******************************************************************************/

/******************************************************************************/
/* Contains Interupt service routine.
 *
/******************************************************************************/

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

#include "PS_2.h"
#include "UART.h"
#include "FLASH.h"
#include "user.h"
#include "Timer.h"
#include "MISC.h"

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* Baseline devices don't have interrupts. Note that some PIC16's
 * are baseline devices.  Unfortunately the baseline detection macro is
 * _PIC12 */
#ifndef _PIC12


/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

extern unsigned char PS_2_ScanCodes[PS2_ScanCode_Limit];
extern unsigned char PS_2_Buffer_items;
extern unsigned int SinLEDtimer;

unsigned int PS_2_Read_Data_TEMP = 0;
unsigned char PS_2_bits = 0;

unsigned char ResendLastFlag = 0;


/******************************************************************************/
/* ISR                                                                */
/******************************************************************************/
void interrupt isr(void)
{
    unsigned char rx;
    unsigned char Rx_fault =0;
    unsigned char ScanTemp;

    if(IOCAF & CLK)
    {
        //Change on PS_2 clk pin
        PS_2_DISABLE_INTERRUPT(CLK);
        PIE1bits.TMR2IE = 0;//disable timer 2 interrupt
        PIE1bits.RCIE = 0;  //disable UART receive interrupt
        LATC |= KeyLED;
        PS_2_bits++;
        PS_2_Read_Data_TEMP <<= 1;
        NOP();
        if(READ_PS_2_PIN(DATA))
        {
            PS_2_Read_Data_TEMP++;
        }
        if(PS_2_bits == PS_2_NUMBITS)
        {
            if(PS_2_Buffer_items < PS2_ScanCode_Limit)
            {
                ScanTemp = (unsigned char) PS2RawToScan(PS_2_Read_Data_TEMP);
                if(ScanTemp)
                {
                    // Passed checksum
                    PS_2_ScanCodes[PS_2_Buffer_items] = ScanTemp;
                    PS_2_Buffer_items++;
                }
                else
                {
                    // Resend last character
                    ResendLastFlag = 1;
                }
                
            }
            PS_2_bits = 0;            
            PS_2_Read_Data_TEMP = 0;
        }
        Timer2Reset();
        PIR1bits.TMR2IF = 0;
        PIE1bits.TMR2IE = 1;//enable timer 2 interupt
        Timer2ON();
        IOCAF &= ~CLK; /* Clear the flag */
        PS_2_ENABLE_INTERRUPT(CLK);
    }
    else if (IOCAF & DATA)
    {
        PS_2_DISABLE_INTERRUPT(DATA);
        IOCAF &= ~DATA; /* Clear the flag */
    }
    else if (PIR1bits.RCIF)
    {
        //rx interrupt
        #ifndef ARDUINO
        LATC |= SinLED;
        SinLEDtimer = 0;
        Rx_fault = 0;
        if(RC1STAbits.FERR)
        {
            Rx_fault =1;
        }
        rx = ReadUSART(); //read the byte from rx register
        #ifdef RS232
        if(READ_CONFIG_PIN())
        {
            if(Rx_fault == 1)
            {
                if(rx == 0)
                {
                    //UART_send_break();
                    UART_send_break_timed(100000);
                }
            }
            else
            {
                UARTchar(rx, YES ,RC1STAbits.RX9D);
            }
        }
        #else
        if(Rx_fault == 1)
        {
            if(rx == 0)
            {
                //UART_send_break();
                UART_send_break_timed(100000);
            }
        }
        else
        {
            UARTchar(rx, YES ,RC1STAbits.RX9D);
        }
        #endif
        #endif
        PIR1bits.RCIF = 0;
    }
    else if(PIR1bits.TMR2IF)
    {
        //Not used in this version
        PIE1bits.TMR2IE = 0;//disable timer 2 interupt
        Timer2OFF();
        PS_2_DISABLE_INTERRUPT(CLK);
        PS_2_bits = 0;
        PS_2_Read_Data_TEMP = 0;
        if(ResendLastFlag)
        {
            // Send the Resend command
            ResendLast();
            ResendLastFlag = 0;
        }
        else
        {
            Process_PS2_ScanCode();
        }
        #ifndef ARDUINO
        PIE1bits.RCIE = 1;//rx interrupt enable
        #endif
        PS_2_ENABLE_INTERRUPT(CLK);
        PIR1bits.TMR2IF =0;
    }
}
#endif


