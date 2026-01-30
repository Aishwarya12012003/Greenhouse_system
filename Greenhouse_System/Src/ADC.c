/********************************************************************
		Created:	31:10:2014   12:16
		Filename: ADC.c
		

		Purpose:	Conversion of Analog voltage into Digital
		Version:	1.0 (Initial release)
		MCU Platform: ARM 7 (LPC2148/38)
*********************************************************************/

/********************************************************************
ADC

Converts Analog voltage present at the selected channel into 10bit 
(typecast into 16 bit) Digital data  & returns it to caller.

********************************************************************/

#include "ADC.h"
#include "HardwareProfile.h"

//U16 previousADCData;


//===================================================================
//Function name     : void ADCInitialize()
//Description       : Initializes inbuilt 10 bit ADC in its featured mode
//Parameters passed : None
//Return parameters : None
//Dependencies      : 
//====================================================================
void ADC0Initialize(void)
{
	AD0CR = 0;
	SetBit(AD0CR, 21);								// Enable ADC module, Set PDN in AD0CR
	
	// Set CLKDIV as 3, clock to ADC will be 12MHz/(3+1) = 3MHz
	SetBit(AD0CR, 8);
	SetBit(AD0CR, 9);
	SetBit(AD0CR, 10);
	SetBit(AD0CR, 11);
}

void ADC1Initialize(void)
{
	AD1CR = 0;
	SetBit(AD1CR, 21);								// Enable ADC module, Set PDN in AD0CR
	
	// Set CLKDIV as 3, clock to ADC will be 12MHz/(3+1) = 3MHz
	SetBit(AD1CR, 8);
	SetBit(AD1CR, 9);
	SetBit(AD1CR, 10);
	SetBit(AD1CR, 11);
}

//===================================================================
//Function name     : U8 ADCRead(U8 adcChannel)
//Description       : Reads Analog data present at selected channel out of 8 
//						channels & returns 10 bits (typecast to 16 bit) of
//						digital data to the caller function.
//Parameters passed : U8 adcChannel (Channel number of required ADC input)
//Return parameters : None
//Dependencies      : 
//====================================================================
U16 ADC0Read(U8 adcChannel)
{
	U16 adcData = 0;
	
//		SetBit(AD0CR, 21);								// Enable ADC module, Set PDN in AD0CR

		AD0CR &= 0xFFFFFF00;								// Clear ADC channel bits

		SetBit(AD0CR, adcChannel);								// Select ADC channel. Set value of SEL (bit 7:0) of AD0CR SFR 

		if(TestBit(AD0GDR , (unsigned long)31) == 0)			// Check if prev conversion is completed
		{
			SetBit(AD0CR, 24);				// Set START bit-> start ADC conversion

			while(!TestBit(AD0GDR , (unsigned long)31));					//Wait for conversion completion, check DONE bit in AD0GDR

			ClearBit(AD0CR, 24);
			adcData = (AD0GDR >> 6) & 0x3FF;
		}
		
		else
		{
			adcData = 0;
		}
	return(adcData);
}

U16 ADC1Read(U8 adcChannel)
{
	U16 adcData = 0;
	
		SetBit(AD1CR, 21);								// Enable ADC module, Set PDN in AD1CR

		AD1CR &= 0xFFFFFF00;								// Clear ADC channel bits

		SetBit(AD1CR, adcChannel);								// Select ADC channel. Set value of SEL (bit 7:0) of AD1CR SFR 

		if(TestBit(AD1GDR , (unsigned long)31) == 0)			// Check if prev conversion is completed
		{
			SetBit(AD1CR, 24);				// Set START bit-> start ADC conversion

			while(!TestBit(AD1GDR , (unsigned long)31));					//Wait for conversion completion, check DONE bit in AD1GDR

			ClearBit(AD1CR, 24);				// Set START bit-> start ADC conversion
			adcData = (AD1GDR >> 6) & 0x3FF;
		}
		
		else
		{
			adcData = 0;
		}
	return(adcData);
}
