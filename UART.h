/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 01/21/15     1.2         Created log.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef UART_H
#define	UART_H

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

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* Uart character and word spacing
 *
 * This parameter modifies the amount of time waited between characters and
 *   words when printing over the UART.
/******************************************************************************/

#define Character_Spacing   5000

/*
 * A typical Character_Spacing value for fast printing is 5000. For slow
 *   printing 200,000 is common.
 */

#define Word_Spacing        15000

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define YES                 1
#define NO                  0
#define USART_TX_INT_ON   		0b10000000  // Transmit interrupt on
#define USART_TX_INT_OFF  		0b00000000  // Transmit interrupt off
#define USART_TX_INT_MASK		(~USART_TX_INT_ON)	//Mask Trnasmit Interrupt select bit
#define USART_RX_INT_ON   		0b01000000  // Receive interrupt on
#define USART_RX_INT_OFF  		0b00000000  // Receive interrupt off
#define USART_RX_INT_MASK		(~USART_RX_INT_ON)	//Mask Receive Interrupt select bit
#define USART_ADDEN_ON    		0b00100000  // Enables address detection
#define USART_ADDEN_OFF   		0b00000000  // Disables address detection
#define USART_ADDEN_MASK		(~USART_ADDEN_ON)	//Mask address detection select bit
#define USART_BRGH_HIGH   		0b00010000  // High baud rate
#define USART_BRGH_LOW    		0b00000000  // Low baud rate
#define USART_BRGH_MASK			(~USART_BRGH_HIGH)	//Mask baud rate select bit
#define USART_CONT_RX     		0b00001000  // Continuous reception
#define USART_SINGLE_RX   		0b00000000  // Single reception
#define USART_CONT_RX_MASK		(~USART_CONT_RX) 	//Mask Continuous Reception select bit
#define USART_SYNC_MASTER 		0b00000100  // Synchrounous master mode
#define USART_SYNC_SLAVE  		0b00000000  // Synchrounous slave mode
#define USART_SYNC_MASK			(~USART_SYNC_MASTER)	//Mask usart mode select bit
#define USART_NINE_BIT    		0b00000010  // 9-bit data
#define USART_EIGHT_BIT   		0b00000000  // 8-bit data
#define USART_BIT_MASK 			(~USART_NINE_BIT)		//Mask 9 bit transmit  select bit
#define USART_SYNCH_MODE  		0b00000001  // Synchronous mode
#define USART_ASYNCH_MODE 		0b00000000  // Asynchronous mode
#define USART_MODE_MASK			(~USART_SYNCH_MODE) 	//Mask sync/async mode select bit
#define USART_BRG16  		0b00001000  // Synchronous mode
#define SENDB                   0b00001000
#define BusyUSART( ) (!TX1STAbits.TRMT)


/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

unsigned long BAUD = 9600;
unsigned char PARITY = 0;

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/

void InitUART(unsigned long Baud, unsigned char parity);
void UARTstring(const unsigned char *data);
void UARTchar(unsigned char data, unsigned char override, unsigned char NinethBit_override_data);
void OpenUSART( unsigned char config, unsigned int spbrg);
char ReadUSART(void);
void baudUSART (unsigned char baudconfig);
void OpenUSART( unsigned char config, unsigned int spbrg);
void UART_send_break_timed(unsigned int US);
void UART_send_break(void);
void UARTstringWAIT(const unsigned char *data);
void SetBaud(unsigned long Baud, unsigned char Parity);
void CloseUSART(void );
unsigned char READ_CONFIG_PIN(void);

/******************************************************************************/
/* Printf messages                                                            */
/******************************************************************************/
const unsigned char NoParityMSG[]   = {" with No parity bit"};
const unsigned char OddParityMSG[]  = {" with Odd parity bit"};
const unsigned char EvenParityMSG[] = {" with Even parity bit"};
const unsigned char MarkParityMSG[] = {" with Mark bit"};
const unsigned char SpaceParityMSG[] = {" with Space bit"};


#endif	/* UART_H */