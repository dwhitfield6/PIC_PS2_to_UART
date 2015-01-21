/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---         -------------------------------------------------------
 * 01/09/15     C           Written.
 *
/******************************************************************************/

/******************************************************************************/
/* Contains Functions for UART
 *
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
#include <stdio.h>         /* For sprintf definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "UART.h"          /* User funct/params, such as InitApp */
#include "MISC.h"          /* User funct/params, such as InitApp */
#include "FLASH.h"          /* User funct/params, such as InitApp */

extern unsigned char BAUDMODE;

void CloseUSART(void )
{
    RCSTA&=0b01001111;
    TX1STAbits.TXEN=0;
    PIE1&=0b11001111;
}

void InitUART(unsigned long Baud, unsigned char parity)
{
unsigned long temp;
unsigned char config;
unsigned int spbrg;
unsigned char baudconfig;
/*
 * parity options
 * 0 is none
 * 1 is Odd
 * 2 is Even
 * 3 is mark
 * 4 is space
 */

LATC |= TX;
PPSLOCK =0;//unlock PPS
RC2PPS = 0b00000000;//RC2 is LATxy
PPSLOCK =1; //lock PPS

CloseUSART(); //turn off usart if was previously on
config =0;
baudconfig =0;
     //-----configure USART -----
config |= USART_TX_INT_OFF;
config |= USART_RX_INT_ON;
config |= USART_ASYNCH_MODE;
config |= USART_CONT_RX;//single mode
config |= USART_BRGH_HIGH;
PARITY = parity;
BAUD = Baud;
if(parity)
{
    config |= USART_NINE_BIT;
}
else
{
    config &= ~USART_NINE_BIT;
}
baudconfig |= USART_BRG16;

temp = (SYS_FREQ / Baud) - 1;
temp = (temp >> 2) - 1;
spbrg = (unsigned int)temp;

RCSTAbits.CREN = 0;
OpenUSART(config, spbrg);
baudUSART(baudconfig);
temp = ReadUSART(); //clear the buffer
temp = ReadUSART(); //clear the buffer
temp = ReadUSART(); //clear the buffer
temp = ReadUSART(); //clear the buffer
temp = ReadUSART(); //clear the buffer

PPSLOCK =0;//unlock PPS
RC2PPS = 0b00010100;//RC2 is TX/CK
CKPPS = 0b00010010;//RC2 is TX
RXPPS = 0b00010101;//RC5 is RX
PPSLOCK =1; //lock PPS

PIR1bits.RCIF = 0; //reset RX pin flag
INTCONbits.PEIE = 1; //Enable pheripheral interrupt
}

void baudUSART (unsigned char baudconfig)
{
    BAUD1CON =0;
    BAUD1CON = baudconfig;
}

void OpenUSART( unsigned char config, unsigned int spbrg)
{
  TX1STA = 0;           // Reset USART registers to POR state
  RC1STA = 0;

  if(config&0x01)      // Sync or async operation
  {
    TX1STAbits.SYNC = 1;
  }

  if(config&0x02)      // 8- or 9-bit mode
  {
    TX1STAbits.TX9 = 1;
    RC1STAbits.RX9 = 1;
  }

  if(config&0x04)      // Master or Slave (sync only)
  {
    TX1STAbits.CSRC = 1;
  }

  if(config&0x08)      // Continuous or single reception
  {
    RC1STAbits.CREN = 1;
  }
  else
  {
    RC1STAbits.SREN = 1;
  }

  if(config&0x10)      // Baud rate select (asychronous mode only)
  {
    TX1STAbits.BRGH = 1;
  }

  if(config&0x20)  // Address Detect Enable
  {
	 //RCSTAbits.ADEN = 1;
  }

  if(config&0x40)      // Interrupt on receipt
  {
    PIR1bits.RCIF = 0;
    PIE1bits.RCIE = 1;
  }
  else
  {
    PIE1bits.RCIE = 0;
  }

  if(config&0x80)      // Interrupt on transmission
  {
    PIR1bits.TXIF = 0;
    PIE1bits.TXIE = 1;
  }
  else
  {
    PIE1bits.TXIE = 0;
  }

  SPBRG = spbrg;       // Write baudrate to SPBRG1
  SPBRGH = spbrg >> 8; // For 16-bit baud rate generation

  TX1STAbits.TXEN = 1;  // Enable transmitter
  RC1STAbits.SPEN = 1;  // Enable receiver
}

void UARTchar(unsigned char data, unsigned char NinethBit_override, unsigned char NinethBit_override_data)
{

    if(TXSTAbits.TX9)  
    {
        // 9-bit mode
        if(!NinethBit_override)
        {
            //no override 9th bit so we will calculate it
            switch (PARITY)
            {
                case 1:
                    TXSTAbits.TX9D = CheckSum_byte((unsigned int) data, Odd);//Odd parity
                    break;
                case 2:
                    TXSTAbits.TX9D = CheckSum_byte((unsigned int) data, Even);//Even parity
                    break;
                case 3:
                    TXSTAbits.TX9D = 1;//mark
                    break;
                default:
                    TXSTAbits.TX9D = 0;//space
                    break;
            }
        }
        else
        {
            TXSTAbits.TX9D = (NinethBit_override_data & 0x01);
        }
    }
  TXREG = data;      // Write the data byte to the USART
}

void UARTstring(const unsigned char *data)
{
  do
  {  // Transmit a byte
      if(*data != 0)
      {
    UARTchar(*data, NO, 0);
    while(BusyUSART());
      }
  } while( *data++);
}

char ReadUSART(void)		//this function can be removed by macro #define ReadUSART RCREG
{
  char data;   // Holds received data

   if(RCSTAbits.FERR)                 // If a framing error occured
    
  if(RCSTAbits.OERR)                 // If an overrun error occured
    RCSTAbits.CREN = 0;

  data = RCREG;                      // Read data

  return (data);                     // Return the received data
}

void UART_send_break(void)
{
    TX1STA |= SENDB;
    TXREG = 'd';      // arbitrary
}

void UART_send_break_timed(unsigned int US)
{
    
    LATC |= TX;
    PPSLOCK =0;//unlock PPS
    RC2PPS = 0b00000000;//RC2 is LATxy
    PPSLOCK =1; //lock PPS
    
    LATC &= ~TX;
    delayUS(US);
    LATC |= TX;

    PPSLOCK =0;//unlock PPS
    RC2PPS = 0b00010100;//RC2 is TX/CK
    CKPPS = 0b00010010;//RC2 is TX
    RXPPS = 0b00010101;//RC5 is RX
    PPSLOCK =1; //lock PPS
}

void UARTstringWAIT(const unsigned char *data)
{
  do
  {  // Transmit a byte
      delayUS(Character_Spacing);
      if(*data != 0)
      {
    UARTchar(*data, NO, 0);
    while(BusyUSART());
      }
  } while( *data++);
}

void SetBaud(unsigned long Baud, unsigned char Parity)
{
    unsigned char buf[50];
    unsigned long Baudtemp=0;
    unsigned long Paritytemp=0;
    unsigned long temp;
    unsigned char i =0;
    //Program Baud to Flash
    WriteBaud(FLASH_ADDRESS_ROW, Baud, Parity);
    temp = ReadBaud(FLASH_ADDRESS_ROW, 0);
    Baudtemp   = temp & 0x000FFFFF;
    Paritytemp = (unsigned char)((temp & 0x00F00000) >> 20);
    //Read Flash to make sure it was written correctly
    InitUART(Baud, Parity);
    UARTstringWAIT("\r\n");
    delayUS(Word_Spacing);
    if((Baud != Baudtemp) || (Parity != Paritytemp))
    {
        UARTstringWAIT("Program Fail\r\n");
        delayUS(Word_Spacing);
    }

    //blink LED
    for(i =0;i<10;i++)
    {
       LATC |= KeyLED;
       delayUS(10000);
       LATC &= ~KeyLED;
       delayUS(10000);
    }

    if(Parity)
    {
        switch (Parity)
        {
            case 1:
                sprintf(buf,"Baud is %lu with Odd parity bit\r\n",Baud);//Odd parity
                break;
            case 2:
                sprintf(buf,"Baud is %lu with Even parity bit\r\n",Baud);//Even parity
                break;
            case 3:
                sprintf(buf,"Baud is %lu with Mark bit\r\n",Baud);//mark
                break;
            default:
                sprintf(buf,"Baud is %lu with Space bit\r\n",Baud);//Space
                break;
        }
    }
    else
    {
        sprintf(buf,"Baud is %lu with no parity bit\r\n",Baud);
    }
    UARTstringWAIT(buf);
    delayUS(Word_Spacing);
}