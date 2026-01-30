#include "Main.h"

U16 sendXbeeCnt = 0;

extern U8 enableRecMsg1;
U8 Rec_GSM_Data = 0;
U8 Rec_Ph_Data = 0;
U16 sensCnt = 0, smsCnt = 0;

U8 Rec_Other_Data = 0;
extern U8 curSerialDevice;
U8 gsmReady = 0;

U8 dispBuff[15];
U8 *strAdrr = NULL;
U8 cnt = 0;

U32 dhtData = 0;
U16	tmp = 0, hum = 0;
U16 waterLevel = 0, mois = 0;
U8 motn = 0;
U8 mot = 0;

U8 soilPhCmd[] = {0x01, 0x03, 0x00, 0x0d, 0x00, 0x01, 0x15, 0xC9, 0x00};
U16 phVal = 0;

int main (void)
{
	InitializeSystem();
	
//	PrintString(0x80, (U8 *)" Automation in  ");
//	PrintString(0xC0, (U8 *)"Agriculture Sytm");
	PrintString(0x80, (U8 *)"   Green House  ");
	PrintString(0xC0, (U8 *)"Mon & Ctl System");
	
	// Blink LED & buzzer
	SetBit(LED1_OUT, LED1);
	SetBit(LED2_OUT, LED2);
	SetBit(BUZZER_PORT, BUZZER);
	_delay_ms(500);
	ClearBit(LED1_OUT, LED1);
	ClearBit(LED2_OUT, LED2);
	ClearBit(BUZZER_PORT, BUZZER);
	_delay_ms(500);
	
//	USART0_Transmit_String_Logic("UART0", 0);
//	USART1_Transmit_String_Logic("UART1", 0);

	_delay_ms(2500);
	ClearLCD();
	PrintString(0x80, (U8 *)"ARM7 - LPC2148 ");
	_delay_ms(1500);
	ClearLCD();
	
	PrintString(0x80, (U8*)"Initializing GSM..");
	_delay_ms(500);
	
	gsmReady = 0;
	
	if(Init_GSM() == 1)
	{
		SetBit(LED1_PORT, LED1);
		PrintString(0x80, (U8*)"GSM Initialized..");
		_delay_ms(100);
		ClearBit(LED1_PORT, LED1);
		gsmReady = 1;
	}
	else
	{
		PrintString(0x80, (U8*)"GSM Error!!      ");
	}
	
	while(1)
	{
		UartReceiveHandler();		// Serial data reception handling
		
		if(sensCnt > 200)		// Read sensor data within 200 ms loop
		{
			sensCnt = 0;
			SetBit(LED2_OUT, LED2);
			
			// Send pH sensor command
			RS_485_TX_EN();
			_delay_ms(5);
			sendStrLenU1(soilPhCmd, 8);
			_delay_ms(2);
			RS_485_RX_EN();		// pH sensor reception enabled


			// Read water level
			waterLevel = ADC1Read(6);		// Read raw adc value
			waterLevel = GetWaterLevel(waterLevel);	// Convert into some range
			
			
			// Read temp & hum
			dhtData = ReadDHT11_1();
			hum = dhtData >> 24;		// Get High Byte of Humidity
			tmp = (dhtData & 0xFF00) >> 8;		// Get High Byte of Temp
			// Read PIR sensor
			motn = TestBit(PIR_PIN, PIR);
			// Read moisture 
			mois = ADC0Read(7);
			mois = GetMoisture(mois);
			
			// Compare with thresholds
			if(waterLevel <= WATER_LEVEL_THRESH || tmp > 40)
			{
				ToggleBit(BUZZER_PORT, BUZZER);
				ClearBit(RELAY_PORT, RELAY);		// Turn off motor
				mot = 0;
//				PrintString(0xC0, (U8*)"Water level low!");
//				_delay_ms(200);
			}
			else
			{
				ClearBit(BUZZER_PORT, BUZZER);
			}
			//SetBit(RELAY_PORT, RELAY);
			if(waterLevel > WATER_LEVEL_THRESH)
			{
				if(mois >= MOISTURE_THRESH)
				{
					ClearBit(RELAY_PORT, RELAY);
					mot = 0;
				}
				else if(mois < (MOISTURE_THRESH - 50))
				{
					SetBit(RELAY_PORT, RELAY);
					mot = 1;
				}
			}
			
			// Display all sensor values
			ClearLCD();
			// Water level
			itoa((char*)dispBuff, waterLevel, 10);		// Convert integer into ascii
			PrintString(0x80, (U8*)"W:");
			PrintStringWoAddr((U8*)dispBuff);
			// Temperature
			utoa((char*)dispBuff, tmp ,10);
			PrintStringWoAddr((U8 *)" T:");
			PrintStringWoAddr(dispBuff);
			// Humidity
			utoa((char*)dispBuff, hum ,10);
			PrintStringWoAddr((U8 *)" H:");
			PrintStringWoAddr(dispBuff);

			itoa((char*)dispBuff, phVal, 10);		// pH
			PrintString(0xC0, (U8*)"p:");
			PrintStringWoAddr((U8*)dispBuff);

			itoa((char*)dispBuff, motn, 10);		// Motion
			PrintStringWoAddr((U8 *)" M");
			PrintStringWoAddr((U8*)dispBuff);

			itoa((char*)dispBuff, mois ,10);		// Moisture
			PrintStringWoAddr((U8 *)" R");
			PrintStringWoAddr(dispBuff);

			itoa((char*)dispBuff, mot ,10);
			PrintStringWoAddr((U8 *)" T");			// Motor
			PrintStringWoAddr(dispBuff);
			
			ClearBit(LED2_OUT, LED2);
		}
		// Send SMS after 1 min
		if(smsCnt > 60000)
		{
			smsCnt = 0;
			SendSensSms();		// Send values thr sms
		}
		_delay_ms(1);
		sensCnt++;
		smsCnt++;
	}
}

// Use this if different freq multiplication/division required
void PLL_Init(void)
{
  PLL0CON = 0x01;                    //Enable PLL
  PLL0CFG = 0x24;                    //2 = Divide by 2, 4 = Multiply by 5
  PLL0FEED = 0xAA;                //Feed sequence
  PLL0FEED = 0x55;
  
  while(!(PLL0STAT & 0x00000400)); //is locked?
    
  PLL0CON = 0x03;                    //Connect PLL after PLL is locked
  PLL0FEED = 0xAA;                //Feed sequence
  PLL0FEED = 0x55;
  VPBDIV = 0x01;                    // PCLK is same as CCLK i.e.60 MHz
}

void InitializeSystem(void)
{
	MEMMAP = 0x01;				//Map interrupt vectors to Flash (Very IMP)
	VPBDIV = 0x01;				// PCLK same as FOSC (i.e. CCLK) (APBDIV)
//	VPBDIV = 0x00;				// APB bus clock (PCLK) is one fourth of the processor clock (CCLK)
	PLL0CFG = 0;
	PLL1CFG = 0;
	PLL0CON = 0;
	PLL1CON = 0;
	
//	PLL_Init();
	
	IO0DIR = 0;
	IO1DIR = 0;
	IO0PIN = 0;
	IO1PIN = 0;
	PINSEL0 = 0;
	PINSEL1 = 0;
	PINSEL2 = 0;
	
	ClearBit(LED1_OUT, LED1);
	ClearBit(LED2_OUT, LED2);

	SetBit(LED1_DIR, LED1);					// LED dir out
	SetBit(LED2_DIR, LED2);					// LED dir out
	
	MakePinOutput(BUZZER_DDR, BUZZER);
	MakePinOutput(RELAY_DIR, RELAY);
	
	MakePinInput(PIR_DIR, PIR);
	
	SetBit(MOTOR_DIR, MOTOR);
	ClearBit(MOTOR_PORT, MOTOR);
	
	MakePinOutput(SEL_TX_RX_DIR, SEL_TX_RX);		// For RS-485 TX RX
	RS_485_RX_EN();		// Enable reception

	InitLCD();		// Commands to LCD
	
	USART0Initialize(BAUDRATE_9600);		// For GSM
	USART1Initialize(BAUDRATE_9600);		// For pH sensor
	
	SetBit(PINSEL1, 11);												// Set P0.21 as analog i/p AD1.6 for Water level
	SetBit(PINSEL0, 10); SetBit(PINSEL0, 11);		// Set P0.5 as analog i/p AD0.7 for Moisture sensor

	ADC0Initialize();
	ADC1Initialize();
	// Use 12MHz or less crystal. By default crystal freq multiplied by 5.
	// Tick "Memory Layout from Target Dialog" in target options to enable interrupt operations
}

U8 UartReceiveHandler(void)
{
	U8 statusCode = 0;
	U8 *strPtr = 0;
	if(Rec_Ph_Data == 1)
	{
		// Frame format: <adrr cmd val>
		// 
		_delay_ms(10);
		
		// Reply from ph sensor: 0x01 0x03 0x02 0x00 0x47 0xD8 0x15

		strPtr = (U8*)strchr((const char*)receiveBufferUART1, PH_SENS_ADDR);
		if(strPtr != 0)
		{
			_delay_ms(10);
			strPtr++;
			
			if(*strPtr == 0x03)
			{
				//strPtr++;
				phVal = ((U16)strPtr[2] << 8) + strPtr[3];
//				utoa((char*)dispBuff, phVal ,10);
//				USART0_Transmit_String_Logic("\rph Val:", 0);
//				USART0_Transmit_String_Logic(dispBuff, 0);
				statusCode = 1;
			}
		}
		
		Rec_Ph_Data = 0;
		receiveCounterUART1 = 0;
		receiveBufferUART1[0] = 0;
		//RS_485_TX_EN();
	}
	
	if(Rec_GSM_Data == 1)
	{
//		sendStrU1("\rGSM:");
//		sendStrU1(receiveBufferUART0);
//		sendStrU1("\r");
		
		Rec_GSM_Data = 0;
		receiveCounterUART0 = 0;
		receiveBufferUART0[0] = 0;
	}
	return statusCode;
}


U8 smsBuf[250];
const U8 mobNum[] = "9119454991";

void SendSensSms(void)
{
	ClearLCD();
	PrintString(0x80, (U8*)"Sending SMS..");
	
	smsBuf[0] = 0;
	
	strcat(smsBuf, "WL: ");
	itoa((char*)dispBuff, waterLevel, 10);
	strcat(smsBuf, dispBuff);

	strcat(smsBuf, "\r\nTemp: ");
	itoa((char*)dispBuff, tmp, 10);
	strcat(smsBuf, dispBuff);

	strcat(smsBuf, "\r\nHum: ");
	itoa((char*)dispBuff, hum, 10);
	strcat(smsBuf, dispBuff);

	strcat(smsBuf, "\r\npH: ");
	itoa((char*)dispBuff, phVal, 10);
	strcat(smsBuf, dispBuff);

	strcat(smsBuf, "\r\nMotn: ");
	itoa((char*)dispBuff, motn, 10);
	strcat(smsBuf, dispBuff);

	strcat(smsBuf, "\r\nMoist: ");
	itoa((char*)dispBuff, mois, 10);
	strcat(smsBuf, dispBuff);

	strcat(smsBuf, "\r\nMotr: ");
	itoa((char*)dispBuff, mot, 10);
	strcat(smsBuf, dispBuff);

/*	utoa((char*)dispBuff, tmp ,10);
	PrintStringWoAddr((U8 *)" T:");
	PrintStringWoAddr(dispBuff);
	utoa((char*)dispBuff, hum, 10);
	PrintStringWoAddr((U8 *)" H:");
	PrintStringWoAddr(dispBuff);

	itoa((char*)dispBuff, phVal, 10);		// pH
	PrintString(0xC0, (U8*)"p:");
	PrintStringWoAddr((U8*)dispBuff);

	itoa((char*)dispBuff, motn, 10);		// Motion
	PrintStringWoAddr((U8 *)" M");
	PrintStringWoAddr((U8*)dispBuff);

	itoa((char*)dispBuff, mois ,10);		// Moisture
	PrintStringWoAddr((U8 *)" R");
	PrintStringWoAddr(dispBuff);

	itoa((char*)dispBuff, mot ,10);
	PrintStringWoAddr((U8 *)" T");			// Motor
	PrintStringWoAddr(dispBuff);

	dtostrf(latlon[0], 2, 6, latBuff);
	dtostrf(latlon[1], 2, 6, longBuff);
	strcat((char*)smsBuff, (const char*)"Fuel is low.. Pump1: ");
	strcat((char*)smsBuff, (const char*)neabyPetrol1);
	strcat((char*)smsBuff, (const char*)"\r\nPump2: ");
	strcat((char*)smsBuff, (const char*)neabyPetrol2);
	_delay_ms(100);*/
	Send_SMS(smsBuf, 0, (U8*)&mobNum[0]);
	_delay_ms(2000);

	PrintString(0xC0, (U8*)"SMS Sent..");
	_delay_ms(1500);
}

void ISRSerial0Receive(void)
{
	U8 receiveData = 0;

	receiveData = U0IIR;
	receiveData = RX0_REGISTER;			//read the current value in U0's Interrupt Register which also clears it.

	receiveBufferUART0[receiveCounterUART0] = receiveData;
	receiveCounterUART0++;
	receiveBufferUART0[receiveCounterUART0] = 0;
	//Rec_Ph_Data = 1;
	Rec_GSM_Data = 1;
	
	if(receiveCounterUART0 >= MAX_RECEIVE_LENGTH)
		receiveCounterUART0 = 0;
	
	VICVectAddr = 0;
}

void ISRSerial1Receive (void)
{
	U8 receiveData = 0;

	receiveData = U1IIR;
	receiveData = RX1_REGISTER;			//read the current value in U1's Interrupt Register which also clears it.

	receiveBufferUART1[receiveCounterUART1] = receiveData;
	receiveCounterUART1++;
	receiveBufferUART1[receiveCounterUART1] = 0;
	//Rec_GSM_Data = 1;
	Rec_Ph_Data = 1;

	//if(receiveCounterUART1 >= MAX_RECEIVE_LENGTH)
		//receiveCounterUART1 = 0;
	
	VICVectAddr = 0;
}
