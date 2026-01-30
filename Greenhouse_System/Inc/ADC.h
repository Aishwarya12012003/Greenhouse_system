#ifndef ADC_H
#define ADC_H


/********************************************************************
		Created:	16:05:2012   18:08
		Filename: 	ADC.h		

		Purpose:	Header file for ADC
		Version:	1.0 (Initial release)
*********************************************************************/
#include "HardwareProfile.h"

#define		ADC_CH_0		0
#define		ADC_CH_1		1
#define		ADC_CH_2		2
#define		ADC_CH_3		3
#define		ADC_CH_4		4
#define		ADC_CH_5		5
#define		ADC_CH_6		6
#define		ADC_CH_7		7

#define		ADC_SUCCESS		0
#define		ADC_BUSY		1
#define		ADC_OPEN_CKT	2
#define		ADC_SHORT_CKT	3

#define		ADC_MAX_COUNT	220

void ADC0Initialize(void);
void ADC1Initialize(void);
U16 ADC0Read(U8 adcChannel);
U16 ADC1Read(U8 adcChannel);

#endif
