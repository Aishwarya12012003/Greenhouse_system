/********************************************************************
		Created:	19-03-2015 08:17:26 PM
		Modified:	12-02-2016: Added 4-bit Databus Support
					01-08-2016: Added Random Pins for Display Port
					25-12-2016: Added Random pins for Four Bit Mode;
								Added pin dir init in LCD.c
					24-01-2017: Added display string through Flash
								PrintStringFlash(...)
		Filename: 	USART.c	


*********************************************************************/

/********************************************************************
LCD

Gives the display facility with 16x2 LCD module.
Platform: ATmega8, ATmega16, ATmega32
********************************************************************/
#ifdef Green_Solutions

#include "LCD.h"

#ifdef USE_LCD_16X2

#if	(!defined(LCD_INTERFACE) || !defined(LCD_NIBBLE_PORT) || !defined(LCD_PIN_MODE))
	#error "Error in this file!!"
#endif

void InitLCD()
{
	ClearBit(LCD_PORT_RS, LCD_RS);
	ClearBit(LCD_PORT_EN, LCD_EN);
	#ifdef LCD_RW
	ClearBit(LCD_PORT_RW, LCD_RW);
	#endif
	
	MakePinOutput(LCD_DIR_RS, LCD_RS);
	MakePinOutput(LCD_DIR_EN, LCD_EN);
	#ifdef LCD_RW
	MakePinOutput(LCD_DIR_RW, LCD_RW);
	#endif
	
	#if (LCD_INTERFACE == EIGHT_BIT_INTERFACE)
	
		#if (LCD_PIN_MODE == LCD_USE_SEQ_PINS)
			LCD_DATABUS_PORT = 0;
			LCD_DATABUS_DIR = PORT_DIR_OUT;
			
		#elif (LCD_PIN_MODE == LCD_USE_RANDOM_PINS)
			ClearBit(LCD_PORT_DB0, LCD_DATA_DB0);
			ClearBit(LCD_PORT_DB1, LCD_DATA_DB1);
			ClearBit(LCD_PORT_DB2, LCD_DATA_DB2);
			ClearBit(LCD_PORT_DB3, LCD_DATA_DB3);
			ClearBit(LCD_PORT_DB4, LCD_DATA_DB4);
			ClearBit(LCD_PORT_DB5, LCD_DATA_DB5);
			ClearBit(LCD_PORT_DB6, LCD_DATA_DB6);
			ClearBit(LCD_PORT_DB7, LCD_DATA_DB7);
			
			MakePinOutput(LCD_DIR_DB0, LCD_DATA_DB0);
			MakePinOutput(LCD_DIR_DB1, LCD_DATA_DB1);
			MakePinOutput(LCD_DIR_DB2, LCD_DATA_DB2);
			MakePinOutput(LCD_DIR_DB3, LCD_DATA_DB3);
			MakePinOutput(LCD_DIR_DB4, LCD_DATA_DB4);
			MakePinOutput(LCD_DIR_DB5, LCD_DATA_DB5);
			MakePinOutput(LCD_DIR_DB6, LCD_DATA_DB6);
			MakePinOutput(LCD_DIR_DB7, LCD_DATA_DB7);

		#endif
		
	#elif (LCD_INTERFACE == FOUR_BIT_INTERFACE)
			ClearBit(LCD_PORT_DB4, LCD_DATA_DB4);
			ClearBit(LCD_PORT_DB5, LCD_DATA_DB5);
			ClearBit(LCD_PORT_DB6, LCD_DATA_DB6);
			ClearBit(LCD_PORT_DB7, LCD_DATA_DB7);
			
			MakePinOutput(LCD_DIR_DB4, LCD_DATA_DB4);
			MakePinOutput(LCD_DIR_DB5, LCD_DATA_DB5);
			MakePinOutput(LCD_DIR_DB6, LCD_DATA_DB6);
			MakePinOutput(LCD_DIR_DB7, LCD_DATA_DB7);
		
	#endif
	
	_delay_ms(20);
	
	#if (LCD_INTERFACE == EIGHT_BIT_INTERFACE)
		SendLCDCommand(0x38);

	#elif (LCD_INTERFACE == FOUR_BIT_INTERFACE)
		SendLCDCommand(0x20);
		_delay_ms(5);
		SendLCDCommand(0x28);
		_delay_ms(5);
		SendLCDCommand(0x20);
		_delay_ms(5);
		SendLCDCommand(0x28);
		_delay_ms(5);
	#endif

	_delay_ms(5);
	SendLCDCommand(0x0C);
	_delay_us(200);
	SendLCDCommand(0x06);
	_delay_us(200);
	SendLCDCommand(0x14);
	_delay_us(200);
	SendLCDCommand(0x01);
	_delay_ms(5);
}

void PutOnLCDDatabus(unsigned char val)
{
	#if (LCD_INTERFACE == EIGHT_BIT_INTERFACE)
		
		#if (LCD_PIN_MODE == LCD_USE_SEQ_PINS)
		
		LCD_DATABUS_PORT = val;
		
		#elif (LCD_PIN_MODE == LCD_USE_RANDOM_PINS)
		
		ClearBit(LCD_PORT_DB0, LCD_DATA_DB0);
		ClearBit(LCD_PORT_DB1, LCD_DATA_DB1);
		ClearBit(LCD_PORT_DB2, LCD_DATA_DB2);
		ClearBit(LCD_PORT_DB3, LCD_DATA_DB3);
		ClearBit(LCD_PORT_DB4, LCD_DATA_DB4);
		ClearBit(LCD_PORT_DB5, LCD_DATA_DB5);
		ClearBit(LCD_PORT_DB6, LCD_DATA_DB6);
		ClearBit(LCD_PORT_DB7, LCD_DATA_DB7);
		
		LCD_PORT_DB0 |= ((val & 0x01) << LCD_DATA_DB0);
		val = val >> 1;
		LCD_PORT_DB1 |= ((val & 0x01) << LCD_DATA_DB1);
		val = val >> 1;
		LCD_PORT_DB2 |= ((val & 0x01) << LCD_DATA_DB2);
		val = val >> 1;
		LCD_PORT_DB3 |= ((val & 0x01) << LCD_DATA_DB3);
		val = val >> 1;
		LCD_PORT_DB4 |= ((val & 0x01) << LCD_DATA_DB4);
		val = val >> 1;
		LCD_PORT_DB5 |= ((val & 0x01) << LCD_DATA_DB5);
		val = val >> 1;
		LCD_PORT_DB6 |= ((val & 0x01) << LCD_DATA_DB6);
		val = val >> 1;
		LCD_PORT_DB7 |= ((val & 0x01) << LCD_DATA_DB7);
		
		#endif
		
		SetBit(LCD_PORT_EN, LCD_EN);
		_delay_us(50);
		ClearBit(LCD_PORT_EN, LCD_EN);
		_delay_us(15);
	
	#elif (LCD_INTERFACE == FOUR_BIT_INTERFACE)
	
		#if (LCD_PIN_MODE == LCD_USE_SEQ_PINS)
		
			LCD_DATABUS_PORT &= NIBBLE_PORT;						// Clear lower nibble which connected to D4-7
		
			#if (LCD_NIBBLE_PORT == USE_HIGH_NIB_PORT)
				LCD_DATABUS_PORT |= (val & 0xF0);				// Send value upper nibble
			#elif (LCD_NIBBLE_PORT == USE_LOW_NIB_PORT)
				LCD_DATABUS_PORT |= (val >> 4);					// Send value upper nibble
			#endif
		
			SetBit(LCD_PORT_EN, LCD_EN);
			_delay_us(25);
			ClearBit(LCD_PORT_EN, LCD_EN);
			_delay_us(15);
	
			LCD_DATABUS_PORT &= NIBBLE_PORT;				// Clear lower nibble which connected to D4-7
		
			#if (LCD_NIBBLE_PORT == USE_HIGH_NIB_PORT)
				LCD_DATABUS_PORT |= ((val << 4) & 0xF0);				// Send value lower nibble
			#elif (LCD_NIBBLE_PORT == USE_LOW_NIB_PORT)
				LCD_DATABUS_PORT |= (val & 0x0F);					// Send value lower nibble
			#endif
		
		#elif (LCD_PIN_MODE == LCD_USE_RANDOM_PINS)
			
			U8 tmpVal = val;
			// Clear all 4 data pins
			ClearBit(LCD_PORT_DB4, LCD_DATA_DB4);
			ClearBit(LCD_PORT_DB5, LCD_DATA_DB5);
			ClearBit(LCD_PORT_DB6, LCD_DATA_DB6);
			ClearBit(LCD_PORT_DB7, LCD_DATA_DB7);
			
			// Send value upper nibble
			val = val >> 4;
			LCD_PORT_DB4 |= ((val & 0x01) << LCD_DATA_DB4);
			val = val >> 1;
			LCD_PORT_DB5 |= ((val & 0x01) << LCD_DATA_DB5);
			val = val >> 1;
			LCD_PORT_DB6 |= ((val & 0x01) << LCD_DATA_DB6);
			val = val >> 1;
			LCD_PORT_DB7 |= ((val & 0x01) << LCD_DATA_DB7);

			SetBit(LCD_PORT_EN, LCD_EN);
			_delay_us(25);
			ClearBit(LCD_PORT_EN, LCD_EN);
			_delay_us(15);

			// Again clear all 4 data pins
			ClearBit(LCD_PORT_DB4, LCD_DATA_DB4);
			ClearBit(LCD_PORT_DB5, LCD_DATA_DB5);
			ClearBit(LCD_PORT_DB6, LCD_DATA_DB6);
			ClearBit(LCD_PORT_DB7, LCD_DATA_DB7);
			
			// Send value lower nibble
			val = tmpVal;
			LCD_PORT_DB4 |= ((val & 0x01) << LCD_DATA_DB4);
			val = val >> 1;
			LCD_PORT_DB5 |= ((val & 0x01) << LCD_DATA_DB5);
			val = val >> 1;
			LCD_PORT_DB6 |= ((val & 0x01) << LCD_DATA_DB6);
			val = val >> 1;
			LCD_PORT_DB7 |= ((val & 0x01) << LCD_DATA_DB7);

		#endif
		
//		LCD_DATABUS_PORT |= (val & ~NIBBLE_PORT);		// Send value lower nibble
		SetBit(LCD_PORT_EN, LCD_EN);
		_delay_us(25);
		ClearBit(LCD_PORT_EN, LCD_EN);
		_delay_us(15);
	
	#endif
}

void SendLCDCommand(unsigned char cmd)
{
	#ifdef LCD_RW
	ClearBit(LCD_PORT_RW, LCD_RW);
	#endif // LCD_RW
	
	ClearBit(LCD_PORT_RS, LCD_RS);
	PutOnLCDDatabus(cmd);
}

void SendLCDData(unsigned char Dbyte)
{
	#ifdef LCD_RW
	ClearBit(LCD_PORT_RW, LCD_RW);
	#endif // LCD_RW
	
	SetBit(LCD_PORT_RS, LCD_RS);
	PutOnLCDDatabus(Dbyte);
}

void ClearLCD(void)
{
	SendLCDCommand(0x01);
	_delay_ms(5);
}

void PrintString(unsigned char lcdAddr, unsigned char *strPtr)
{
	unsigned char count = 0;

	SendLCDCommand(lcdAddr);
	_delay_us(50);

	while(strPtr[count] != 0)
	{
		SendLCDData(strPtr[count]);
		count++;
	}
}

// Expected string as PSTR type
/*void PrintStringFlash(unsigned char lcdAddr, unsigned char* string)
{
	SendLCDCommand(lcdAddr);
	_delay_us(50);

	while (pgm_read_byte(&(*string)))
	SendLCDData(pgm_read_byte(&(*string++)));
}*/

void PrintStringWoAddr(unsigned char *strPtr)
{
	unsigned char count = 0;

	while(strPtr[count] != 0)
	{
		SendLCDData(strPtr[count]);
		count++;
	}
}

void PrintStringLength(unsigned char lcdAddr, unsigned char *strPtr, unsigned char strLen)
{
	unsigned char count = 0;

	SendLCDCommand(lcdAddr);
	_delay_us(50);

	for(count = 0; count < strLen; count++)
	{
		SendLCDData(strPtr[count]);
	}
}

void _delay_us(U32 delay)
{
	U16 d;

  for (; delay > 0; delay--)           /* only to delay for LED flashes */
	{
		for (d = 10; d > 0; d--);
	}
}

void _delay_ms(U16 delay)
{
	U16 d;

  for (; delay > 0; delay--)           /* only to delay for LED flashes */
	{
		  //for (d = 0; d < 7500; d++);
		  for (d = 0; d < 8900; d++);		// 20MHz crystal

	}
}

#endif		// USE_LCD_16X2
#endif
