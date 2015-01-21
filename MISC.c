/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---         -------------------------------------------------------
 * 01/09/15     C           Taken from proj6_UART_RF_ADC_LCD_RTC.
 *
/******************************************************************************/

/******************************************************************************/
/* Contains Miscellaneous Functions
 *
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


/* DelayUs
 * Input: number in microseconds to be delayed
 *
 * The actual delay can be scewed when interruots are enabled
 */
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

void cleanBuffer(unsigned char* data, int count)
{
    unsigned char i=0;
    for(i=0; i<count;i++)
    {
        data[i]= 0;
    }
}

void BufferCopy(unsigned char* from,unsigned char* to, unsigned char count, unsigned char shift)
{
    unsigned char i=0;
    cleanBuffer(to,count);
    for(i = shift; i>0;i--)
    {
        *to = ' ';
        to++;
    }
    while(*from != 0 && count >0)
    {
    *to = *from;
    from++;
    to++;
    count--;
    }
}

unsigned char StringMatch(unsigned char* This,const unsigned char* That)
{
    while(*This != 0)
    {
       if(*This != *That || *That == 0)
       {
           return 0;
       }
       This++;
       That++;
    }
    if(*That == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned char StringContains(unsigned char* This, const unsigned char* That)
{
    while(*This != 0)
    {
       while(*This == *That)
       {
           That++;
           This++;
           if(*That == 0)
           {
               return 1;
           }
           if(*This == 0)
           {
               return 0;
           }
       }
       This++;
    }
    return 0;
}

unsigned char StartsWith(unsigned char* This,const unsigned char* That)
{
    while(*This != 0)
    {
        if(*This != *That)
        {
            if(*That != 0)
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
        This++;
        That++;
    }
    if(*This == 0 && *That == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int GetEnteredNumber(unsigned char* This)
{
    unsigned char i =0;
    int temp =0;
    unsigned char negative =0;
    while(*This != '=' && *This != 0)
    {
        This++;
        //go all the way to the equal
    }
    if(*This == 0)
    {
        return 32000; //return the largest unsigned int to show error
                    // there is no equal
    }
    This++;
    if(*This == ' ')
    {
      This++;
      //move past the space
    }
    else if (*This == '=')
    {
         return 32002; //double equal
    }
    else if(*This == '-')
    {
      negative = 1;
      This++;
      //move past the space
    }
    else if(*This == 0)
    {
        return 32001; //return the largest unsigned int to show error
                    // there is no value after the equal
    }
    while(*This != 0)
    {
        temp = temp * 10;
        if (*This >= 48 && *This <= 57)
        {
            temp+= *This - 48;
            i++;
        }
        else
        {
            return 32003;
            //letter mixed in with number
        }
        if(i>5)
        {
             return 32004;
            //number too big
        }
        This++;
    }
    if(negative)
    {
        temp = (temp * -1);
    }
    return temp;

}
void RemoveSpaces(unsigned char* This)
{
    //must match size of *This which is 100
    unsigned char i =0;
    unsigned char* address = This;
    unsigned char That[10];
    cleanBuffer(That,10);
    while(*This != 0)
    {
        if(*This != ' ')
        {
            That[i] = *This;
            i++;
        }
        This++;
    }
    BufferCopy(That,address, 10, 0);
}
void lowercase(unsigned char* Input)
{
    unsigned char i =0;
    while((Input[i]!= 0) && (i < 10))
    {
        if((Input[i] >= 65) && (Input[i] <= 90 ))
        {
            Input[i] += 32;
        }
        i++;
    }
}
unsigned char StringAddEqual(unsigned char* Input)
{
    unsigned char i =0;
    unsigned char j =0;
    unsigned char firstnumber =200;
    unsigned char temp[10];
    cleanBuffer(temp,10);
    while(Input[i] != 0)
    {
            if((Input[i] >= 48) && (Input[i] <= 57) || (Input[i] == '-'))
            {
                firstnumber=i;
                break;
            }
            i++;
    }
    if(firstnumber ==200)
    {
        return 0;
    }
    for(j=0;j<firstnumber;j++)
    {
           temp[j] = Input[j];
    }
    temp[firstnumber] = '=';
    for(j = (firstnumber);j<99;j++)
    {
        temp[j+1] = Input[j];
        if(Input[j] ==0)
        {
            break;
        }
    }
    BufferCopy(temp,Input,100, 0);
    return 1;
}

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