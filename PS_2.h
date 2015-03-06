/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 01/21/15     1.2         Created log.
 * 05/05/15     1.3_DW0a    Decreased timeout for keypress to increase
 *                            throughput.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef PS_2_H
#define	PS_2_H

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

/******************************************************************************/
/* PS/2 scan code Timer
 *
 * This parameter modifies the timeout. If the value is too small and then we
 *  get repeat letters. If it is too big then we miss scan codes. Largest is
 *  255.
/******************************************************************************/

#define Read_Timer_Timeout 50

/******************************************************************************/
/* PS2_ScanCode_Limit
 *
 * Maximum number of Scan code items currently in the que.
/******************************************************************************/

#define PS2_ScanCode_Limit 12

/******************************************************************************/
/* KeyboardConnectedTimeout
 *
 * Length of loopcounts to wait before checking if the keyboard is connected.
/******************************************************************************/

#define KeyboardConnectedTimeout 1000

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define PS_2_CLK_TRIS	TRISAbits.TRISA5
#define PS_2_DATA_TRIS	TRISAbits.TRISA4
#define PS_2_CLK 0b00100000 // RA5
#define PS_2_DATA 0b00010000 // RA4
#define CLK 0b00100000
#define DATA 0b00010000
#define PS_2_NUMBITS   11
#define PS_2_send_timeout 15000
#define Connected_Wait    5000
#define TRUE 1
#define FALSE 0

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
unsigned char PS_2_ScanCodes[PS2_ScanCode_Limit];
unsigned char PS_2_Buffer_items = 0;
unsigned int PS_2_Read_Data_First =0;
unsigned int PS_2_Read_Data_Second =0;
unsigned int PS_2_Read_Data_Third =0;

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/

void Clock_TRIS(unsigned char direction);
void Data_TRIS(unsigned char direction);
void INIT_PS_2_INTERRUPT(unsigned char DATA_CLK);
unsigned char READ_PS_2_PIN(unsigned char DATA_CLK);
void Process_PS2_ScanCode(void);
void PS_2_INIT(void);
unsigned char Decode_Scan_Code(unsigned char Code);
unsigned char Decode_Scan_Code_Shift(unsigned char Code);
unsigned char Decode_Scan_Code_FunctionE0(unsigned char Code);
unsigned char Decode_Scan_Code_Function(unsigned char Code);
void PS_2_ENABLE_INTERRUPT(unsigned char DATA_CLK);
void PS_2_DISABLE_INTERRUPT(unsigned char DATA_CLK);
unsigned char Send_PS2(unsigned char command);
unsigned char Init_PS_2_Send(void);
#ifndef ReducedMem
unsigned char Keyboard_Connected(void);
#endif
unsigned int PS2RawToScan(unsigned int Raw);
void ResendLast(void);

//offset is 13
const unsigned char ScanCode[113] = {
0x09    ,//TAB
0x60    ,//"  `  "
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'q'     ,
'1'     ,
'\0'    ,
'\0'    ,
'\0'    ,
'z'     ,
's'     ,
'a'     ,
'w'     ,
'2'     ,
'\0'    ,
'\0'    ,
'c'     ,
'x'     ,
'd'     ,
'e'     ,
'4'     ,
'3'     ,
'\0'    ,
'\0'    ,
0x20    ,//space
'v'     ,
'f'     ,
't'     ,
'r'     ,
'5'     ,
'\0'    ,
'\0'    ,
'n'     ,
'b'     ,
'h'     ,
'g'     ,
'y'     ,
'6'     ,
'\0'    ,
'\0'    ,
'\0'    ,
'm'     ,
'j'     ,
'u'     ,
'7'     ,
'8'     ,
'\0'    ,
'\0'    ,
0x2C    ,// " , "
'k'     ,
'i'     ,
'o'     ,
'0'     ,
'9'     ,
'\0'    ,
'\0'    ,
'.'     ,
0x2F    ,// " /"
'l'     ,
';'     ,
'p'     ,
'-'     ,
'\0'    ,
'\0'    ,
'\0'    ,
0x27    ,// "  ' "
'\0'    ,
'['     ,
'='     ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\r'    ,
']'     ,
'\0'    ,
0x5C    ,// " \ "
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
0x08    ,// backspace
'\0'    ,
'\0'    ,
'1'     ,
'\0'    ,
'4'     ,
'7'     ,
'\0'    ,
'\0'    ,
'\0'    ,
'0'     ,
'.'     ,
'2'     ,
'5'     ,
'6'     ,
'8'     ,
0x1B    ,//escape
'\0'    ,
'\0'    ,
'+'     ,
'3'     ,
'-'     ,
'*'     ,
'9'     };

//offset is 13
const unsigned char ScanCodeSHIFT[113] = {
0x09    ,//TAB
'~'     ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'Q'     ,
'!'     ,
'\0'    ,
'\0'    ,
'\0'    ,
'Z'     ,
'S'     ,
'A'     ,
'W'     ,
'@'     ,
'\0'    ,
'\0'    ,
'C'     ,
'X'     ,
'D'     ,
'E'     ,
'$'     ,
'#'     ,
'\0'    ,
'\0'    ,
0x20    ,//space
'V'     ,
'F'     ,
'T'     ,
'R'     ,
'%'     ,
'\0'    ,
'\0'    ,
'N'     ,
'B'     ,
'H'     ,
'G'     ,
'Y'     ,
'^'     ,
'\0'    ,
'\0'    ,
'\0'    ,
'M'     ,
'J'     ,
'U'     ,
'&'     ,
'*'     ,
'\0'    ,
'\0'    ,
'<'     ,
'K'     ,
'I'     ,
'O'     ,
')'     ,
'('     ,
'\0'    ,
'\0'    ,
'>'     ,
'?'     ,
'L'     ,
':'     ,
'P'     ,
'_'     ,
'\0'    ,
'\0'    ,
'\0'    ,
0x22    ,// "  " "
'\0'    ,
'{'     ,
'+'     ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\r'    ,
'}'     ,
'\0'    ,
'|'     ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
0x08    ,// backspace
'\0'    ,
'\0'    ,
'1'     ,
'\0'    ,
'4'     ,
'7'     ,
'\0'    ,
'\0'    ,
'\0'    ,
'0'     ,
'.'     ,
'2'     ,
'5'     ,
'6'     ,
'8'     ,
0x1B    ,//escape
'\0'    ,
'\0'    ,
'+'     ,
'3'     ,
'-'     ,
'*'     ,
'9'     };

const unsigned char ScanCodeFunctionNoE0[132] = {
'\0'    ,
0x11    ,//F9
'\0'    ,
0x0C    ,//F5
0x0A    ,//F3
0x06    ,//F1
0x07    ,//F2
0x14    ,//F12
'\0'    ,
0x12    ,//F10
0x10    ,//F8
0x0E    ,//F6
0x0B    ,//F4
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
0x1C    ,//L ALT
'\0'    ,
'\0'    ,
0x1A    ,//L CTRL
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
0x13    ,//F11
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
0x0F    };//F7

//offset is 17
const unsigned char ScanCodeFunctionE0[109] = {
0x1E    ,//R Alt
'\0'    ,
'\0'    ,
0x1F    ,//R control
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
0x19    ,//L Gui
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
0x1D    ,//R Gui
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
0x2F    ,// " / "
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\r'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
0x01    ,//end
'\0'    ,
0x03    ,//left arrow
0x16    ,//Home
'\0'    ,
'\0'    ,
'\0'    ,
0x15    ,//Insert
0x7F    ,//delete
0x04    ,//down arrow
'\0'    ,
0x05    ,//right arrow
0x02    ,//up arrow
'\0'    ,
'\0'    ,
'\0'    ,
'\0'    ,
0x18    ,//page down
'\0'    ,
'\0'    ,
0x17    };//page down


#endif	/* PS_2_H */