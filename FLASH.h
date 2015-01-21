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

#endif

#include "system.h"

#define FLASH_ADDRESS 0x1800
#define FLASH_ADDRESS_ROW (FLASH_ADDRESS >> 5)//address is FLASH_ADDRESS >> 5

#define RD    0b00000001
#define WR    0b00000010
#define WREN  0b00000100
#define WRERR 0b00001000
#define FREE  0b00010000
#define LWLO  0b00100000
#define CFGS  0b01000000

unsigned int MEM[2] =
{
    0,
    9600,
};

unsigned int ReadFlash_2Byte(unsigned int row, unsigned char column);
unsigned long ReadFlash_4Byte(unsigned int row, unsigned char column);
void UNLOCK(void);
void BlockErase(unsigned int row);
void WriteFlash_Sequential(unsigned int row, unsigned int* buffer, char amount);
void WriteBaud(unsigned int row, unsigned long Baud, unsigned char Parity);
unsigned long ReadBaud(unsigned int row, unsigned char column);