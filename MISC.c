/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 01/09/15     1.0C           Taken from proj6_UART_RF_ADC_LCD_RTC.
 * 03/03/15     1.3_DW0a       Define out the config code when product is an
 *                               Arduino.
 *
/******************************************************************************/

/******************************************************************************/
/* Contains Miscellaneous Functions
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

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "MISC.h"
#include "system.h"
#include "user.h"

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* DelayUs
 * Input the number in microseconds to be delayed.
 *
 * The function waists loops for the entered bumber of cycles.
 * The actual delay can be scewed when interrupts are enabled.
/******************************************************************************/
void delayUS(long US)
{
    long i;
    unsigned char j;
    for(i=0;i<US;i+=delayConst)
    {
        for(j=0;j<SYS_FREQ_US;j++)
        {
          NOP();
        }
    }
}

/******************************************************************************/
/* CheckSum_byte
 *
 * The function reads the value of 'This' and retunrn the checksum. The Odd_Even
 * parameter determines if it returns an even polarity bit or an odd polarity
 * bit.
/******************************************************************************/
unsigned char CheckSum_byte(unsigned int This, unsigned char Odd_Even)
{
    char i = 0;
    unsigned char Parity = 0;
    unsigned int ThisTemp = This;

    for (i =0; i<8; i++)
    {
        if((ThisTemp & 0x01) == 1)
        {
            Parity++;
        }
        ThisTemp >>= 1;
    }
    if(Odd_Even == Odd)
    {
        if(Parity % 2 == 0) //even
        {
            return 1;
        }
        return 0;
    }
    else
    {
        if(Parity % 2 == 1) //odd
        {
            return 1;
        }
        return 0;
    }
}

/******************************************************************************/
/* Reverse_Byte
 *
 * The function reads the value of 'This' and returns the reverse of the data.
/******************************************************************************/
unsigned char Reverse_Byte(unsigned char This)
{
    unsigned char temp=0;

    temp += (This & 0x01) << 7;
    temp += (This & 0x02) << 5;
    temp += (This & 0x04) << 3;
    temp += (This & 0x08) << 1;
    temp += (This & 0x10) >> 1;
    temp += (This & 0x20) >> 3;
    temp += (This & 0x40) >> 5;
    temp += (This & 0x80) >> 7;

    return temp;
}

/******************************************************************************/
/* ISNUMBER
 *
 * The function returns TRUE if the ascii value is associated with a number.
/******************************************************************************/
unsigned char ISNUMBER(unsigned char ascii)
{
    if(ascii >= '0' && ascii <='9')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/******************************************************************************/
/* unsigned char READ_CONFIG_PIN(void)
 *
 * The function returns configuration based off of the config pin.
/******************************************************************************/
unsigned char READ_CONFIG_PIN(void)
{
        //READ RC0
        #ifndef ARDUINO
        if((PORTC & Config) == Config)
        {
            return (1);
        }
        #endif
        return 0;
}

/******************************************************************************/
/* cleanBuffer
 *
 * This function sets an amount of data in the array as 0.
/******************************************************************************/
void cleanBuffer(unsigned char* data, int count)
{
    unsigned char i=0;
    for(i=0; i<count;i++)
    {
        data[i]= 0;
    }
}

/******************************************************************************/
/* BufferShiftBack 
 *
 * This function shifts all of the elements in the buffer to the left the
 * designated amount of times. The Buffer is filled with zeros on the right
 * end.
/******************************************************************************/
unsigned char BufferShiftBack(unsigned char* buffer, unsigned char shift, unsigned char size)
{
    unsigned char i=0;
    if(shift > size)
    {
        return FAIL;
    }
    for(i = 0; i < size; i++)
    {
        if(i < (size - shift))
        {
            buffer[i] = buffer[i+shift];
        }
        else
        {
            buffer[i] = 0;
        }
    }
    return PASS;
}