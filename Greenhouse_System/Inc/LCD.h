
#ifndef LCD_H
#define LCD_H
#ifdef Green_Solutions

//#include <avr/pgmspace.h>
#include "HardwareProfile.h"

void InitLCD(void);
void SendLCDCommand(unsigned char cmd);
void SendLCDData(unsigned char Dbyte);
void ClearLCD(void);
void PrintString(unsigned char lcdAddr, unsigned char *strPtr);
void PrintStringFlash(unsigned char lcdAddr, unsigned char* string);
void PrintStringWoAddr(unsigned char *strPtr);
void PrintStringLength(unsigned char lcdAddr, unsigned char *strPtr, unsigned char strLen);
void PutOnLCDDatabus(unsigned char val);
void _delay_us(U32 delay);
void _delay_ms(U16 delay);


#define		LCD_INTERFACE		EIGHT_BIT_INTERFACE//FOUR_BIT_INTERFACE//
#define		LCD_NIBBLE_PORT		USE_LOW_NIB_PORT
#define		LCD_PIN_MODE		LCD_USE_RANDOM_PINS //LCD_USE_SEQ_PINS
#define		NIBBLE_PORT			0xF0

#define		LCD_RS 					17	//P0.17
//#define		LCD_RW
#define		LCD_EN 					16	//P0.16

#define		LCD_PORT_RS				IO0PIN
#define		LCD_PORT_RW       IO0PIN
#define		LCD_PORT_EN				IO0PIN

#define		LCD_DIR_RS				IO0DIR
#define		LCD_DIR_RW        IO0DIR
#define		LCD_DIR_EN				IO0DIR

//#define		LCD_CONTROL_PORT		PORTA
//#define		LCD_CONTROL_DIR			DDRA

//#define		LCD_DATABUS_PORT		PORTB
//#define		LCD_DATABUS_PIN			PINB
//#define		LCD_DATABUS_DIR			DDRB

#define		LCD_DATA_DB0			15 //P0.15
#define		LCD_DATA_DB1			31//14 //P0.14 // Pin changed from 0.14 to 1.31
#define		LCD_DATA_DB2			13 //P0.13
#define		LCD_DATA_DB3			12 //P0.12
#define		LCD_DATA_DB4			25//11 //P0.11 // Pin changed from 0.11 to 1.25
#define		LCD_DATA_DB5			23 //P1.23
#define		LCD_DATA_DB6			10 //P0.10
#define		LCD_DATA_DB7			24 //P1.24

// For Random Pins for Display Bus
#define		LCD_PORT_DB0			IO0PIN
#define		LCD_PORT_DB1			IO1PIN //IO0PIN
#define		LCD_PORT_DB2			IO0PIN
#define		LCD_PORT_DB3			IO0PIN
#define		LCD_PORT_DB4			IO1PIN //IO0PIN
#define		LCD_PORT_DB5			IO1PIN
#define		LCD_PORT_DB6			IO0PIN
#define		LCD_PORT_DB7			IO1PIN

#define		LCD_DIR_DB0				IO0DIR
#define		LCD_DIR_DB1				IO1DIR //IO0DIR
#define		LCD_DIR_DB2				IO0DIR
#define		LCD_DIR_DB3				IO0DIR
#define		LCD_DIR_DB4				IO1DIR //IO0DIR
#define		LCD_DIR_DB5				IO1DIR
#define		LCD_DIR_DB6				IO0DIR
#define		LCD_DIR_DB7				IO1DIR

// Define LCD_INTERFACE in hardwareprofile.h
#define		FOUR_BIT_INTERFACE		0x04
#define		EIGHT_BIT_INTERFACE		0x08

// Define LCD_NIBBLE_PORT in hardwareprofile.h
#define		USE_HIGH_NIB_PORT		0x0F
#define		USE_LOW_NIB_PORT		0xF0

// Define LCD_PIN_MODE in hardwareprofile.h
#define		LCD_USE_RANDOM_PINS		0x10
#define		LCD_USE_SEQ_PINS		0x11

#endif
#endif
