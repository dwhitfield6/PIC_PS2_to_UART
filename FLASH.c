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
/* Contains Internal Flash Memory Functions
 *
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "FLASH.h"
#include "user.h"
#include "MISC.h"

unsigned int ReadFlash_2Byte(unsigned int row, unsigned char column)
{
    PMCON1 &= ~CFGS;
    PMADRH = (unsigned char)((row & 0x3F8) >> 3);
    PMADRL = (unsigned char)(((row & 0x007) << 5) + column);
    PMCON1 |= RD;
    while(PMCON1 & RD);
    NOP();
    NOP();
    return (PMDATL | (PMDATH << 8));
}

unsigned long ReadFlash_4Byte(unsigned int row, unsigned char column)
{
    unsigned long temp =0;
    temp =  ((unsigned long)ReadFlash_2Byte(row, column) << 16);
    temp |= ((unsigned long)ReadFlash_2Byte(row, (column + 1)));
    return temp;
}

void UNLOCK(void)
{
    PMCON2 = 0x55;
    PMCON2 = 0xAA;
    PMCON1 |= WR;
    while(PMCON1 & WR);
    NOP();
    NOP();
}

void BlockErase(unsigned int row)
{
    INTCON &= ~gie;//disable global interrupts
    PMCON1 &= ~CFGS;
    PMADRH = (unsigned char)((row & 0x3F8) >> 3);
    PMADRL = (unsigned char)((row & 0x007) << 5);
    PMCON1 |= FREE;
    PMCON1 |= WREN;
    UNLOCK();
    PMCON1 &= ~WREN;
    INTCON |= gie;//enable global interrupts
}
void WriteFlash_Sequential(unsigned int row, unsigned int* buffer, char amount)
{
    unsigned char spaces = 32;
    unsigned char tempH;
    unsigned char tempL;
    unsigned char i;
    unsigned int temp;
    unsigned char amountTemp =0;
    unsigned char TempBufH[32];
    unsigned char TempBufL[32];

    for(i=0;i<spaces;i++)
    {
        amountTemp++;
        if(amountTemp <= amount)
        {
            tempH = (unsigned char)((*buffer & 0x3F00) >> 8);
            tempL = (unsigned char)(*buffer & 0x00FF);
            buffer++;
        }
        else
        {
            //reput what was already there
            temp = ReadFlash_2Byte(row, i);
            tempH = (unsigned char)((temp & 0x3F00) >>8);
            tempL = (unsigned char)(temp & 0x00FF);
        }
        TempBufH[i] = tempH;
        TempBufL[i] = tempL;
    }
    BlockErase(row);
    delayUS(1000);
    INTCON &= ~gie;//disable global interrupts
    PMCON1 |= WREN;
    PMCON1 &= ~CFGS;
    PMCON1 &= ~LWLO;
    PMCON1 &= ~FREE;
    PMADRH = (unsigned char)((row & 0x3F8) >> 3);
    PMADRL = (unsigned char)((row & 0x007) << 5);

    for(i=0;i<spaces;i++)
    {
        PMDATH = TempBufH[i];
        PMDATL = TempBufL[i];
        NOP();
        NOP();
        UNLOCK();
        PMADRL++;

    }
    PMCON1 &= ~WREN;
    INTCON |= gie;//enable global interrupts
}

void WriteBaud(unsigned int row, unsigned long Baud, unsigned char Parity)
{
    unsigned long temp;
    temp = (Baud & 0x000FFFFF) + (((unsigned long) Parity & 0x7) <<  20);
    MEM[1] = (unsigned int) (temp & 0x0003FFF);
    MEM[0] = (unsigned int)((temp & 0xFFFC000) >> 14);
    WriteFlash_Sequential(row, MEM, 2);
}
unsigned long ReadBaud(unsigned int row, unsigned char column)
{
    unsigned long temp;
    temp =(ReadFlash_4Byte(row, column));
    MEM[1] = (unsigned int)(temp & 0x0003FFF);
    MEM[0] = (unsigned int)((temp & 0xFFFC000) >> 14);
    return (((unsigned long)MEM[0] << 12) + (unsigned long)MEM[1]);
}