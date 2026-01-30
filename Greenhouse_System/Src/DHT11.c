/*
 * DHT11.c
 *
 * Created: 09-01-2017 04:01:07 PM
 *  Author: Kaustubh Gabale
 */ 

/********************************************************************
DHT11

Reads the Humidity & Temperature values from DHT11 Sensor Module.
Platform: ATmega8, ATmega16, ATmega32
********************************************************************/

#ifdef EmbedPro_Solutions

#include "DHT11.h"

#ifdef	USE_DHT11
/************************************************************************
 Function Name      : U32 ReadDHT11_1(void)
 Description        : Reads Humidity 16 bit (8 bit integer + 8 bit decimal) data
					  + Temperature 16 bit (8 bit integer + 8 bit fractional) data
					  to give total 32 bit data. (Humidity + Temperature)
 Parameters Returned: U32 : Humidity (16 bit) + Temperature (16 bit)
*************************************************************************/
U32 ReadDHT11_1(void)
{
	U8 readCnt = 0, measTime = 0;
	U32 readData = 0;
	U32 timeOut = 0;
	
	MakePinOutput(HUMID1_DIR, HUMID1);
	ClearBit(HUMID1_PORT, HUMID1);
	_delay_ms(20);
	MakePinInput(HUMID1_DIR, HUMID1);
	
	do
	{
		timeOut++;
		_delay_us(1);
	}
	while(HUMID1_IN() && timeOut < 100000);		// Wait for getting Pull Low from Sensor or for 100ms timeout
	
	if(timeOut >= 100000)
	{
		return 0;		// No response from Sensor
	}

	while(!HUMID1_IN());		// Wait for getting Pull High from Sensor
	while(HUMID1_IN());			// Wait for getting Pull Low from Sensor

	for (readCnt = 0; readCnt < 32; readCnt++)
	{
		while(!HUMID1_IN());		// Wait till low val (50us)
		while(HUMID1_IN())
		{
			measTime++;
			_delay_us(1);
		}
		
		readData = readData << 1;
//		if(measTime >= 40)
		if(measTime >= 20)
		{
			readData |= 1;
		}
		else
		{
			readData |= 0;
		}
		measTime = 0;
	}
	
	return readData;
}

#if DHT11_NUM > 1
U32 ReadDHT11_2(void)
{
	U8 readCnt = 0, measTime = 0;
	U32 readData = 0;
	U32 timeOut = 0;
	
	MakePinOutput(HUMID2_DIR, HUMID2);
	ClearBit(HUMID2_PORT, HUMID2);
	_delay_ms(20);
	MakePinInput(HUMID2_DIR, HUMID2);
	
	do
	{
		timeOut++;
		_delay_us(1);
	}
	while(HUMID2_IN() && timeOut < 100000);		// Wait for getting Pull Low from Sensor or for 100ms timeout
	
	if(timeOut >= 100000)
	{
		return 0;		// No response from Sensor
	}
//	while(HUMID2_IN());
	while(!HUMID2_IN());		// Wait till response signal from sensor
	while(HUMID2_IN());
	
	for (readCnt = 0; readCnt < 32; readCnt++)
	{
		while(!HUMID2_IN());		// Wait till low val (50us)
		while(HUMID2_IN())
		{
			measTime++;
			_delay_us(1);
		}
		
		readData = readData << 1;
		if(measTime >= 50)
		{
			readData |= 1;
		}
		else
		{
			readData |= 0;
		}
		measTime = 0;
	}
	
	return readData;
}
#endif	// #if DHT11_NUM > 1

#endif
#endif
