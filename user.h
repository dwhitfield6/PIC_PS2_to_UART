/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
#define SinLEDTimeOut 100
#define pwrLEDtime    1000
#define BlinkDelay    4

#define VoltageLow 4.65
#define VoltageHigh 5.35

#define ON 1
#define OFF 0
#define OUTPUT 0
#define INPUT 1

#define gie 0x80
#define iocie 0x08

#define TXTRIS	TRISCbits.TRISC2
#define TX 0b00000100 //RC2

#define pwrLEDTRIS	TRISCbits.TRISC1
#define pwrLED 0b00000010 //RC1

#define SinLEDTRIS	TRISCbits.TRISC3
#define SinLED 0b00001000 //RC3

#define KeyLEDTRIS	TRISCbits.TRISC4
#define KeyLED 0b00010000 //RC4

//BatIN is the battery voltage cut in half by the voltage divider
#define BatINTRIS	TRISAbits.TRISA2
#define BatIN 0b00000100 //RA2 OR an2

//DivON turns on the voltage divider to take a reading
#define DivONTRIS	TRISCbits.TRISC0
#define DivON 0b00000001 //RC0

/* TODO User level functions prototypes (i.e. InitApp) go here */

void InitApp(void);         /* I/O and Peripheral Initialization */
void Init_System (void);
