#ifndef MAIN_H
#define MAIN_H

//#include <LPC214x.h>
#include <string.h>
#include "StdLibrary.h"
#include "ADC.h"
#include "USART.h"
//#include "Timer.h"
//#include "ESP8266.h"
#include "LCD.h"
#include "HardwareProfile.h"
//#include "Keypad.h"
#include "GSM.h"
#include "DHT11.h"

void Load_DAC(U16 dacVal);
void InitializeSystem(void);
void IntToStr(U16 dd,U8 *str);
U16 GetHeartRate(void);
void _delay_ms(U16 delay);
void ClearOtpBuff(void);
U8 Rfid_Rec_Handler(void);
void ClearDataBase(void);
//void Display_Database(U8* dataBase);
void Display_Database(U8 AccNo);
void PLL_Init(void);
U8 UartReceiveHandler(void);
void SendSensSms(void);

#define MAX_DATA_SIZE		200

// Water level: empty = 400, full = 900

#define		WATER_LEVEL_THRESH		10

#define   MAX_VOL     100		// y2 : Actual max volume (%)
#define   MIN_VOL     0			// y1 : Actual min volume (%)
#define   FULL_VAL   700		// x2 : Sensor reading @ max volume
#define   EMPTY_VAL  400		// x1 : Sensor reading @ min volume
// m = (y1 - x1) / (y2 - x2)
#define   m           ((float)(MAX_VOL - MIN_VOL) / (FULL_VAL - EMPTY_VAL))
// c = y - m * x
#define   c           ((float)MIN_VOL - (m * EMPTY_VAL))
// y = m * x + c
#define   GetWaterLevel(wl) ((float)(m * wl) + c)

// Moisture: wet = 410, dry = 722
#define		MOISTURE_THRESH		90

#define   MAX_MOIS     	100		// y2 : Actual max volume (%)
#define   MIN_MOIS     	0			// y1 : Actual min volume (%)
#define   MOIS_MAX_VAL  500		// x2 : Sensor reading @ max moisture i.e. dry
#define   MOIS_MIN_VAL  722		// x1 : Sensor reading @ min moisture ie wet
// m = (y1 - x1) / (y2 - x2)
#define   m_m           ((float)(MAX_MOIS - MIN_MOIS) / (float)(MOIS_MAX_VAL - MOIS_MIN_VAL))
// c = y - m * x
#define   c_m           ((float)MIN_MOIS - (float)(m_m * MOIS_MIN_VAL))
// y = m * x + c
#define   GetMoisture(msr) ((float)(m_m * msr) + c_m)

#define PH_SENS_ADDR	0x01




//#define Init_GSM()	1

#endif
