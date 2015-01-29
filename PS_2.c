/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 01/09/15     1.0C        Written.
 * 01/15/15     1.0C        Sequence keyboard leds on initialization.
 * 01/20/15     1.1         Added code to allow for a key to be release at the
 *                            same time of pressing a new key. This allows for
 *                             faster typing.
 *                          Add code to turn on shift key led when shift key is
 *                            hit.
 *                          Add code to try to turn on Keyboard 10 times then
 *                            break.
 *                          Add code to lowercase the letters if caps lock is on
 *                            and shift key is pressed.
 *                          Add code to keep the shift key illuiminated when
 *                            releasing the right key if the left key is still
 *                            pressed and vice versa.
 *                          Fixed code to do caps lock at button press instead
 *                            of release.
 *                          Fixed Baud rate sting logic to save memory.
 *                          Added "keyboard" before every baud rate change.
/******************************************************************************/

/******************************************************************************/
/* Contains Functions for PS/2 Keyboard
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
#include "PS_2.h"
#include "MISC.h"
#include "UART.h"
#include "FLASH.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

extern unsigned int PS_2_Read_Data_FirstTEMP;
extern unsigned int PS_2_Read_Data_SecondTEMP;
extern unsigned int PS_2_Read_Data_ThirdTEMP;
extern unsigned int PS_2_Read_Data_ForthTEMP;
extern unsigned int PS_2_Read_Data_First;
extern unsigned int PS_2_Read_Data_Second;
extern unsigned int PS_2_Read_Data_Third;
extern unsigned int PS_2_Read_Data_Forth;
extern unsigned char PS_2_bits;
extern unsigned char No_Keyboard;
extern unsigned int Read_Timer;

unsigned char i=0;
unsigned char CapsCount=0;
unsigned char Right_Shift_Key =0;
unsigned char Left_Shift_Key =0;
unsigned char Right_Shift_Key_old =0;
unsigned char Left_Shift_Key_old =0;
unsigned char Shift_Key =0;
unsigned char Caps_Lock = 0;
unsigned char Caps_Lock_Key =0;
unsigned char Caps_Lock_Key_old =0;
unsigned char RALT=0;
unsigned char LALT=0;
unsigned char RControl=0;
unsigned char LControl=0;
unsigned char Delete=0;
unsigned char BAUDMODE=0;
volatile unsigned char ECHO=0;
unsigned long BaudTyped=0;
unsigned char ParityTyped=0;

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* PS_2_INIT
 *
 * The function initializes the PS/2 keyboard. The intterupts are sent and the
 *   inputs are set as an input.
/******************************************************************************/
void PS_2_INIT(void)
{
    //turn both pins to inputs and turn on the interrupt
    Clock_TRIS(INPUT);
    Data_TRIS(INPUT);
    INIT_PS_2_INTERRUPT(CLK);
}

/******************************************************************************/
/* Clock_TRIS
 *
 * The function modifys the diretction of the PS/2 clock pin.
/******************************************************************************/
void Clock_TRIS(unsigned char direction)
{
    if(direction == OUTPUT)
    {
        PS_2_CLK_TRIS = OUTPUT;
    }
    else
    {
        PS_2_CLK_TRIS = INPUT;
    }
}

/******************************************************************************/
/* Data_TRIS
 *
 * The function modifys the diretction of the PS/2 data pin.
/******************************************************************************/
void Data_TRIS(unsigned char direction)
{
    if(direction == OUTPUT)
    {
        PS_2_DATA_TRIS = OUTPUT;
    }
    else
    {
        PS_2_DATA_TRIS = INPUT;
    }
}

/******************************************************************************/
/* INIT_PS_2_INTERRUPT
 *
 * The function initialzes the PS_2 interupt associated with the input
 *   'DATA_CLK'.
/******************************************************************************/
void INIT_PS_2_INTERRUPT(unsigned char DATA_CLK)
{
    IOCAP =0;
    IOCAN =0;
    if(DATA_CLK == CLK)
    {
        //clock pin is RA5
        //IOCAP |= CLK;//interrupt on Change positive edge
        IOCAN |= CLK;//interrupt on Change negative edge
    }
    else
    {
        //data pin is RA4
        //IOCAP |= DATA;//interrupt on Change positive edge
        IOCAN |= DATA;//interrupt on Change negative edge
    }
}

/******************************************************************************/
/* PS_2_DISABLE_INTERRUPT
 *
 * The function disables the PS_2 interupt associated with the input 'DATA_CLK'.
/******************************************************************************/
void PS_2_DISABLE_INTERRUPT(unsigned char DATA_CLK)
{
    if(DATA_CLK == CLK)
    {
        //clock pin is RA5
        //IOCAP |= CLK;//interrupt on Change positive edge
        IOCAN &= ~CLK;//disable interrupt on Change negative edge
        IOCAF &= ~CLK;//clear flag
    }
    else
    {
        //data pin is RA4
        //IOCAP |= DATA;//interrupt on Change positive edge
        IOCAN &= ~DATA;//disable interrupt on Change negative edge
        IOCAF &= ~DATA;//clear flag
    }
}

/******************************************************************************/
/* PS_2_ENABLE_INTERRUPT
 *
 * The function enables the PS_2 interupt associated with the input 'DATA_CLK'.
/******************************************************************************/
void PS_2_ENABLE_INTERRUPT(unsigned char DATA_CLK)
{
    if(DATA_CLK == CLK)
    {
        //clock pin is RA5
        //IOCAP |= CLK;//interrupt on Change positive edge
        IOCAF &= ~CLK;//clear flag
        IOCAN |= CLK;//disable interrupt on Change negative edge

    }
    else
    {
        //data pin is RA4
        //IOCAP |= DATA;//interrupt on Change positive edge
        IOCAF &= ~DATA;//clear flag
        IOCAN |= DATA;//disable interrupt on Change negative edge

    }
}

/******************************************************************************/
/* READ_PS_2_PIN
 *
 * The function returns the digital value on the data pin or clock pin.
/******************************************************************************/
unsigned char READ_PS_2_PIN(unsigned char DATA_CLK)
{
    if(DATA_CLK == CLK)
    {
        //READ RA5
        if((PORTA & CLK) == CLK)
        {
            return (1);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        //READ RA4
        if((PORTA & DATA) == DATA)
        {
            return (1);
        }
        else
        {
            return 0;
        }
    }
}

/******************************************************************************/
/* PS_2_Update
 *
 * The function is called around the main loop. This function id polled until
 *   the keyboard character is ready to be decoded. This function is big and
 *   takes a lot of time to complete.
/******************************************************************************/
void PS_2_Update(void)
{
    unsigned char temp =0;
    unsigned char buf[60];
    //no new key pressed
    if(Read_Timer < Read_Timer_Timeout)
    {
        Read_Timer++;
    }
    else
    {
        PS_2_bits = 0;
    }

    if(Read_Timer == Read_Timer_Timeout && PS_2_bits != 0)
    {
        //new key is pressed
        PS_2_DISABLE_INTERRUPT(CLK);
        PIE1bits.RCIE = 0;//rx interrupt disable

        if(PS_2_bits >= PS_2_NUMBITS)
        {
            //single byte key
            //packet received
            PS_2_Read_Data_First = PS_2_Read_Data_FirstTEMP;
            if(CheckSum_byte((PS_2_Read_Data_First >> 2) & 0xFF , Odd) == ((PS_2_Read_Data_First & 0x02)>> 1))
            {
                // CheckSum PASS
                PS_2_Read_Data_First = ((PS_2_Read_Data_First >> 2) & 0xFF);
                PS_2_Read_Data_First = (unsigned int) Reverse_Byte((unsigned char) PS_2_Read_Data_First);
                No_Keyboard = FALSE;
            }
            else
            {
                //bad keycode
                PS_2_Read_Data_First =0;
            }
        }
        if(PS_2_bits >= (PS_2_NUMBITS_2))
        {
            //double byte key
            //packet received
            PS_2_Read_Data_Second = PS_2_Read_Data_SecondTEMP;
            if(CheckSum_byte((PS_2_Read_Data_Second >> 2) & 0xFF , Odd) == ((PS_2_Read_Data_Second & 0x02)>> 1))
            {
                // CheckSum PASS
                PS_2_Read_Data_Second = ((PS_2_Read_Data_Second >> 2) & 0xFF);
                PS_2_Read_Data_Second = (unsigned int) Reverse_Byte((unsigned char) PS_2_Read_Data_Second);
                No_Keyboard = FALSE;
            }
            else
            {
                //bad keycode
                PS_2_Read_Data_First =0;
                PS_2_Read_Data_Second =0;
            }
        }
        if(PS_2_bits >= (PS_2_NUMBITS_3))
        {
            //three byte key
            //packet received
            PS_2_Read_Data_Third = PS_2_Read_Data_ThirdTEMP;
            if(CheckSum_byte((PS_2_Read_Data_Third >> 2) & 0xFF , Odd) == ((PS_2_Read_Data_Third & 0x02)>> 1))
            {
                // CheckSum PASS
                PS_2_Read_Data_Third = ((PS_2_Read_Data_Third >> 2) & 0xFF);
                PS_2_Read_Data_Third = (unsigned int) Reverse_Byte((unsigned char) PS_2_Read_Data_Third);
                No_Keyboard = FALSE;
            }
            else
            {
                //bad key code
                PS_2_Read_Data_First =0;
                PS_2_Read_Data_Second =0;
                PS_2_Read_Data_Third =0;
            }
        }
        if(PS_2_bits >= (PS_2_NUMBITS_4))
        {
            //this is part of 2 different codes
            //packet received
            PS_2_Read_Data_Forth = PS_2_Read_Data_ForthTEMP;
            if(CheckSum_byte((PS_2_Read_Data_Forth >> 2) & 0xFF , Odd) == ((PS_2_Read_Data_Forth & 0x02)>> 1))
            {
                // CheckSum PASS
                PS_2_Read_Data_Forth = ((PS_2_Read_Data_Forth >> 2) & 0xFF);
                PS_2_Read_Data_Forth = (unsigned int) Reverse_Byte((unsigned char) PS_2_Read_Data_Forth);
                No_Keyboard = FALSE;
            }
            else
            {
                //bad key code
                PS_2_Read_Data_First =0;
                PS_2_Read_Data_Second =0;
                PS_2_Read_Data_Third =0;
                PS_2_Read_Data_Forth =0;
            }
        }

        if(PS_2_Read_Data_Forth)
        {
            if(PS_2_Read_Data_First != 0xF0 && PS_2_Read_Data_Second != 0xF0 &&
                    PS_2_Read_Data_Third != 0xF0 && PS_2_Read_Data_Forth != 0xF0)
            {
                //none are released codes
                NOP();
            }
        }
        else if(PS_2_Read_Data_Third)
        {
            //it is a three byte key
            if(PS_2_Read_Data_First == 0xF0 && PS_2_Read_Data_Third != 0xF0 &&
                    PS_2_Read_Data_First != 0xE0 &&
                    PS_2_Read_Data_Second != 0xE0 &&
                    PS_2_Read_Data_Third != 0xE0)
            {
                //Typed a second key before the first one was released
                if(Decode_Scan_Code_Shift(PS_2_Read_Data_Third))
                {
                    //there is a real ascii key for this
                    PS_2_Read_Data_First = PS_2_Read_Data_Third;
                    PS_2_Read_Data_Second = 0;
                    PS_2_Read_Data_Third = 0;
                }
            }
            if(PS_2_Read_Data_First == 0xE0)
            {
                if(PS_2_Read_Data_Second == 0xF0)
                {
                    if(PS_2_Read_Data_Third == 0x11 )//right ALT Key
                    {
                        LALT = 0;
                    }
                    else if(PS_2_Read_Data_Third == 0x14 )//right Control Key
                    {
                        LControl = 0;
                    }
                    else if(PS_2_Read_Data_Third == 0x71 )//delete Key
                    {
                        Delete = 0;
                    }
                }
            }
        }
        else if(PS_2_Read_Data_Second)
        {
            //it is a 2 byte key
            if(PS_2_Read_Data_First == 0xF0)
            {
                //released key
                if(PS_2_Read_Data_Second == 0x12)//right shift released
                {
                    Right_Shift_Key =0;
                }
                else if(PS_2_Read_Data_Second == 0x59)//left shift released
                {
                    Left_Shift_Key =0;
                }
                else if(PS_2_Read_Data_Second == 0x14)//left Control Key
                {
                    RControl =0;
                }
                else if(PS_2_Read_Data_Second == 0x11 )//left ALT Key
                {
                    RALT =0;
                }
                if(PS_2_Read_Data_Second == 0x59 || PS_2_Read_Data_Second == 0x12)
                {
                    if(Right_Shift_Key == 0 && Left_Shift_Key == 0)
                    {
                        if(!Send_PS2(0xED))//Command LED
                        {
                            if(!Send_PS2(0x00))//CAPS LED
                            {
                                Shift_Key = 0;//release shift key
                            }
                        }
                        if(Caps_Lock)
                        {
                            CapsCount=0;
                            while(CapsCount<10)
                            {
                                if(!Send_PS2(0xED))//Command LED
                                {
                                    if(!Send_PS2(0x04))//CAPS LED
                                    {
                                        break;
                                    }
                                }
                                CapsCount++;
                            }
                        }
                        else
                        {
                            CapsCount=0;
                            while(CapsCount<10)
                            {
                                if(!Send_PS2(0xED))//Command LED
                                {
                                    if(!Send_PS2(0x00))//CAPS LED
                                    {
                                        break;
                                    }
                                }
                                CapsCount++;
                            }
                        }
                    }
                }
                else if(PS_2_Read_Data_Second == 0x58)//CAPS Lock
                {
                    Caps_Lock_Key = 0;
                }
            }
            else if(PS_2_Read_Data_First == 0xE0)
            {
                //it has an E0 first
                if(PS_2_Read_Data_Second == 0x11 )//right ALT Key
                {
                    LALT = 1;
                }
                else if(PS_2_Read_Data_Second == 0x14 )//right Control Key
                {
                    LControl = 1;
                }
                else if(PS_2_Read_Data_Second == 0x71 )//delete Key
                {
                    Delete = 1;
                }
                if(((RControl || LControl) + (LALT || RALT) + Delete) != 3)
                {
                    //include delete
                    temp = Decode_Scan_Code_FunctionE0(PS_2_Read_Data_Second);
                }
            }
        }
        if(PS_2_Read_Data_First && PS_2_Read_Data_Second == 0 &&
                PS_2_Read_Data_Third == 0 && PS_2_Read_Data_Forth == 0)
        {
            //one byte key
            if(PS_2_Read_Data_First == 0x12)//Right shift key pressed
            {
                Right_Shift_Key = 1;
            }
            else if(PS_2_Read_Data_First == 0x59)//left shift key pressed
            {
                Left_Shift_Key = 1;
            }
            if((Right_Shift_Key == 1 && Right_Shift_Key_old == 0) || (Left_Shift_Key == 1 && Left_Shift_Key_old == 0))
            {
                //this is a new press
                if(!Send_PS2(0xED))//Command LED
                {
                    if(Caps_Lock)
                    {
                        CapsCount=0;
                        while(CapsCount<10)
                        {
                            if(!Send_PS2(0xED))//Command LED
                            {
                                if(!Send_PS2(0x05))//CAPS LED
                                {
                                    Shift_Key = 1;//release shift key
                                    break;
                                }
                            }
                            CapsCount++;
                        }
                    }
                    else
                    {
                        CapsCount=0;
                        while(CapsCount<10)
                        {
                            if(!Send_PS2(0xED))//Command LED
                            {
                                if(!Send_PS2(0x01))//CAPS LED
                                {
                                    Shift_Key = 1;//release shift key
                                    break;
                                }
                            }
                            CapsCount++;
                        }
                    }
                }
            }
            if(PS_2_Read_Data_First == 0x14 )//left Control Key
            {
                RControl = 1;
            }
            else if(PS_2_Read_Data_First == 0x11 )//left ALT Key
            {
                RALT = 1;
            }
            else if(PS_2_Read_Data_First == 0xEE )//ECHO
            {
                ECHO = 1;
            }
            else if(PS_2_Read_Data_First == 0x58)//CAPS Lock
            {
                Caps_Lock_Key = 1;
                if(Caps_Lock_Key_old == 0)
                {
                    if(Caps_Lock)
                    {
                        if(Shift_Key)
                        {
                            CapsCount=0;
                            while(CapsCount<10)
                            {
                                if(!Send_PS2(0xED))//Command LED
                                {
                                    if(!Send_PS2(0x01))//CAPS LED
                                    {
                                        Caps_Lock =0;
                                        break;
                                    }
                                }
                                CapsCount++;
                            }
                        }
                        else
                        {
                            CapsCount=0;
                            while(CapsCount<10)
                            {
                                if(!Send_PS2(0xED))//Command LED
                                {
                                    if(!Send_PS2(0x00))//CAPS LED
                                    {
                                        Caps_Lock =0;
                                        break;
                                    }
                                }
                                CapsCount++;
                            }
                        }
                    }
                    else
                    {
                        if(Shift_Key)
                        {
                            CapsCount=0;
                            while(CapsCount<10)
                            {
                                if(!Send_PS2(0xED))//Command LED
                                {
                                    if(!Send_PS2(0x05))//CAPS LED
                                    {
                                        Caps_Lock =1;
                                        break;
                                    }
                                }
                                CapsCount++;
                            }
                        }
                        else
                        {
                            CapsCount=0;
                            while(CapsCount<10)
                            {
                                if(!Send_PS2(0xED))//Command LED
                                {
                                    if(!Send_PS2(0x04))//CAPS LED
                                    {
                                        Caps_Lock =1;
                                        break;
                                    }
                                }
                                CapsCount++;
                            }
                        }
                    }
                }

            }
            else
            {
                if(Shift_Key ==1)
                {
                    //shift key is pressed
                    temp = Decode_Scan_Code_Shift(PS_2_Read_Data_First);
                    if(Caps_Lock)
                    {
                        //lowercase the letters
                        if(temp >= 'A' && temp <= 'Z')
                        {
                            temp += 32;
                        }
                    }
                }
                else
                {
                    temp = Decode_Scan_Code(PS_2_Read_Data_First);
                    if(Caps_Lock)
                    {
                        //capitalize the letters
                        if(temp >= 'a' && temp <= 'z')
                        {
                            temp -= 32;
                        }
                    }
                }
                if(!temp)
                {
                    //it isnt an ascii represented value
                    temp = Decode_Scan_Code_Function(PS_2_Read_Data_First);
                }
            }
        }
        if(((RControl || LControl) + (LALT || RALT) + Delete) == 3)
        {
            //control alt delete is pressed at the same time
            UARTstringWAIT("\r\n");
            delayUS(Word_Spacing);
            if(!BAUDMODE)
            {
                BaudTyped =0;
                ParityTyped =0;
                BAUDMODE = 1;
                UARTstringWAIT("Enter Baud rate\r\n");
                delayUS(Word_Spacing);
                UARTchar('>' , NO, 0);

            }
            else
            {
                BaudTyped = 9600;
                UARTstringWAIT("KeyBoard Reset to 9600 with no parity bit\r\n");
                delayUS(Word_Spacing);
                SetBaud(BaudTyped, 0);
                BAUDMODE = 0;
            }
            RControl =0;
            LControl =0;
            LALT =0;
            RALT =0;
            Delete =0;
        }
        if(temp)
        {
            //there is a value to print
            if(temp == 0x01)
            {
                //break character
                //UART_send_break();
                UART_send_break_timed(100000);
            }
            else
            {
                if(!BAUDMODE)
                {
                    UARTchar(temp, NO, 0);
                    if(temp == '\r')
                    {
                        delayUS(Character_Spacing);
                        UARTchar('\n', NO, 0);
                        delayUS(Word_Spacing);
                    }
                }
                else
                {
                    if(ISNUMBER(temp))
                    {
                        BaudTyped*=10;
                        BaudTyped += temp - 48;
                        UARTchar(temp, NO, 0);
                    }
                    else if(temp == 'o' || temp == 'O')
                    {
                        ParityTyped = 1;//odd parity
                        UARTchar(temp, NO, 0);
                    }
                    else if(temp == 'e' || temp == 'E')
                    {
                        ParityTyped = 2;//even parity
                        UARTchar(temp, NO, 0);
                    }
                    else if(temp == 'm' || temp == 'M')
                    {
                        ParityTyped = 3;//mark
                        UARTchar(temp, NO, 0);
                    }
                    else if(temp == 's' || temp == 'S')
                    {
                        ParityTyped = 4;//space
                        UARTchar(temp, NO, 0);
                    }
                    else if(temp == '\r')
                    {
                        UARTstringWAIT("\r\n");
                        delayUS(Word_Spacing);
                        if(BaudTyped >=2400 && BaudTyped <= 115200)
                        {
                            sprintf(buf,"KeyBoard Baud will be set to %lu",BaudTyped);
                            UARTstringWAIT(buf);
                            if(ParityTyped)
                            {
                                switch (ParityTyped)
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
                            SetBaud(BaudTyped, ParityTyped);
                        }
                        else
                        {
                            UARTstringWAIT("KeyBoard Baud Out of Range\r\n");
                            delayUS(Word_Spacing);
                        }
                        BAUDMODE=0;
                    }
                }
            }
        }
        PS_2_Read_Data_First =0;
        PS_2_Read_Data_Second =0;
        PS_2_Read_Data_Third =0;
        PS_2_Read_Data_Forth =0;
        PS_2_Read_Data_FirstTEMP =0;
        PS_2_Read_Data_SecondTEMP =0;
        PS_2_Read_Data_ThirdTEMP =0;
        PS_2_Read_Data_ForthTEMP =0;
        Caps_Lock_Key_old = Caps_Lock_Key;
        Right_Shift_Key_old = Right_Shift_Key;
        Left_Shift_Key_old = Left_Shift_Key;
        LATC &= ~KeyLED;
        PIE1bits.RCIE = 1;//rx interrupt enable
        PS_2_ENABLE_INTERRUPT(CLK);
    }
}

/******************************************************************************/
/* Decode_Scan_Code
 *
 * The function returns the ascii character associated with the scan code.
/******************************************************************************/
unsigned char Decode_Scan_Code(unsigned char Code)
{
    Code -= 13;
    if(Code <=112)
    {
        return ScanCode[Code];
    }
    else
    {
        return 0;
    }
}

/******************************************************************************/
/* Decode_Scan_Code_Shift
 *
 * The function returns the ascii character associated with the scan code.
 *   These ascii values are used when the shift key is held.
/******************************************************************************/
unsigned char Decode_Scan_Code_Shift(unsigned char Code)
{
    Code -= 13;
    if(Code <=112)
    {
        return ScanCodeSHIFT[Code];
    }
    else
    {
        return 0;
    }
}

/******************************************************************************/
/* Decode_Scan_Code_FunctionE0
 *
 * The function returns the artificial ascii character associated with the scan
 *   code. These scan codes are 2 part scan codes. The first byte is E0.
/******************************************************************************/
unsigned char Decode_Scan_Code_FunctionE0(unsigned char Code)
{
    Code -= 17;
    if(Code <=108)
    {
        return ScanCodeFunctionE0[Code];
    }
    else
    {
        return 0;
    }
}

/******************************************************************************/
/* Decode_Scan_Code_Function
 *
 * The function returns the artificail ascii character associated with the scan
 *   code.
/******************************************************************************/
unsigned char Decode_Scan_Code_Function(unsigned char Code)
{
    if(Code <=131)
    {
        return ScanCodeFunctionNoE0[Code];
    }
    else
    {
        return 0;
    }
}

/******************************************************************************/
/* Send_PS2
 *
 * The function sends a command to the PS/2 keyboard.
/******************************************************************************/
unsigned char Send_PS2(unsigned char command)
{
    unsigned char i=0;
    unsigned char oddparity = CheckSum_byte((unsigned int)command , Odd);
    unsigned int timeout =0;

    Clock_TRIS(OUTPUT);
    Data_TRIS(OUTPUT);
    LATA &= ~PS_2_CLK;//bring clock low
    delayUS(300);
    LATA &= ~PS_2_DATA;//bring data low
    delayUS(20);
    Clock_TRIS(INPUT);//release clock
    timeout=0;
    while(READ_PS_2_PIN(CLK))//wait till clock is brought low
    {
        NOP();
        timeout++;
        if(timeout > PS_2_send_timeout)
        {
            Clock_TRIS(INPUT);
            Data_TRIS(INPUT);
            return 1;
        }
    }
    for(i=0;i<8;i++)
    {
        if(command & 0x1)
        {
            LATA |= PS_2_DATA;//bring data low
        }
        else
        {
            LATA &= ~PS_2_DATA;//bring data low
        }
        command >> = 1;
        timeout=0;
        while(!READ_PS_2_PIN(CLK))//wait till clock is brought high
        {
            NOP();
            timeout++;
            if(timeout > PS_2_send_timeout)
            {
                Clock_TRIS(INPUT);
                Data_TRIS(INPUT);
                return 1;
            }
        }
        timeout=0;
        while(READ_PS_2_PIN(CLK))//wait till clock is brought high
        {
            NOP();
            timeout++;
            if(timeout > PS_2_send_timeout)
            {
                Clock_TRIS(INPUT);
                Data_TRIS(INPUT);
                return 1;
            }
        }
    }
    //send parity
    if(oddparity & 0x1)
    {
        LATA |= PS_2_DATA;//bring data low
    }
    else
    {
        LATA &= ~PS_2_DATA;//bring data low
    }
    timeout=0;
    while(!READ_PS_2_PIN(CLK))//wait till clock is brought high
    {
            NOP();
            timeout++;
            if(timeout > PS_2_send_timeout)
            {
                Clock_TRIS(INPUT);
                Data_TRIS(INPUT);
                return 1;
            }
    }
    timeout=0;
    while(READ_PS_2_PIN(CLK))//wait till clock is brought high
    {
            NOP();
            timeout++;
            if(timeout > PS_2_send_timeout)
            {
                Clock_TRIS(INPUT);
                Data_TRIS(INPUT);
                return 1;
            }
    }
    
    Data_TRIS(INPUT);//release data line
    
    timeout=0;
    while(READ_PS_2_PIN(DATA))//wait till data is brought low
    {
            NOP();
            timeout++;
            if(timeout > PS_2_send_timeout)
            {
                Clock_TRIS(INPUT);
                Data_TRIS(INPUT);
                return 1;
            }
    }
    timeout=0;
    while(READ_PS_2_PIN(CLK))//wait till clock is brought low
    {
            NOP();
            timeout++;
            if(timeout > PS_2_send_timeout)
            {
                Clock_TRIS(INPUT);
                Data_TRIS(INPUT);
                return 1;
            }
    }
    timeout=0;
    while(!READ_PS_2_PIN(DATA))//wait till data is brought HIGH
    {
            NOP();
            timeout++;
            if(timeout > PS_2_send_timeout)
            {
                Clock_TRIS(INPUT);
                Data_TRIS(INPUT);
                return 1;
            }
    }
    timeout=0;
    while(!READ_PS_2_PIN(CLK))//wait till clock is brought HIGH
    {
            NOP();
            timeout++;
            if(timeout > PS_2_send_timeout)
            {
                Clock_TRIS(INPUT);
                Data_TRIS(INPUT);
                return 1;
            }
    }
    
    //pass
    Clock_TRIS(INPUT);
    Data_TRIS(INPUT);
    PS_2_ENABLE_INTERRUPT(CLK);
    return 0;
}

/******************************************************************************/
/* Init_PS_2_Send
 *
 * The function flashes the scroll light, num lock light, and caps lock
 *   light once. The keyboard is also comanded to be enabled and to use scan
 *   code set 1.
/******************************************************************************/
unsigned char Init_PS_2_Send(void)
{
    PS_2_DISABLE_INTERRUPT(CLK);
    
    //scroll LED
    if(Send_PS2(0xED))//Command LED
    {
        return 0;
    }
    delayUS(100);
    if(Send_PS2(0x01))//Turn on scroll LED
    {
        return 0;
    }
    delayUS(Character_Spacing);

    //Caps lock LED
    if(Send_PS2(0xED))//Command LED
    {
        return 0;
    }
    delayUS(100);
    if(Send_PS2(0x04))//Turn on Caps LED
    {
        return 0;
    }
    delayUS(Character_Spacing);

    //Num lock LED
    if(Send_PS2(0xED))//Command LED
    {
        return 0;
    }
    delayUS(1000);
    if(Send_PS2(0x02))//Turn on Number lock LED
    {
        return 0;
    }
    delayUS(Character_Spacing);
    
    //LEDs Off
    if(Send_PS2(0xED))//Command LED
    {
        return 0;
    }
    delayUS(100);
    if(Send_PS2(0x00))//Turn on scroll LED
    {
        return 0;
    }

    //Enable Keyboard
    if(Send_PS2(0xf4))
    {
        return 0;
    }
    
    //Enable Keyboard
    if(Send_PS2(0xf4))
    {
        return 0;
    }
    
    //Scan Code Set
    if(Send_PS2(0xf0))
    {
        return 0;
    }
    delayUS(100);
    if(Send_PS2(0x02))
    {
        return 0;
    }
    
    //Repeat rate
    if(Send_PS2(0xf3))
    {
        return 0;
    }
    delayUS(100);
    if(Send_PS2(0x2F))//15 Hz with delay of 500ms
    {
        return 0;
    }
    
    PS_2_ENABLE_INTERRUPT(CLK);
    return 1;
}

/******************************************************************************/
/* Keyboard_Connected
 *
 * The function checks to see if the keyboard is connected. The Echo signal is
 *   waited on.
/******************************************************************************/
unsigned char Keyboard_Connected(void)
{
    unsigned long i=0;
    ECHO =0;
    Send_PS2(0xEE);//Echo
    PS_2_ENABLE_INTERRUPT(CLK);
    for(i=0;i<Connected_Wait;i++)
    {
        PS_2_Update();
        delayUS(10);
        if(ECHO)
        {
            break;
        }
    }
    PS_2_DISABLE_INTERRUPT(CLK);
    if(ECHO)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}