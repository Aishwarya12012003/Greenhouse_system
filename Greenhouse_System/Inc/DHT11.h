/*
 * DHT11.h
 *
 * Created: 09-01-2017 04:07:15 PM

 */ 


#ifndef DHT11_H_
#define DHT11_H_

#ifdef Green_Solutions

#include "HardwareProfile.h"

U32 ReadDHT11_1(void);
U32 ReadDHT11_2(void);

#define		DHT11_NUM			1

#define		HUMID1				19	//P1.19
#define		HUMID2				3

#define		HUMID1_PORT			IO1PIN
#define		HUMID2_PORT			IO0PIN

#define		HUMID1_DIR			IO1DIR
#define		HUMID2_DIR			IO0DIR

#define		HUMID1_PIN			IO1PIN
#define		HUMID2_PIN			IO0PIN

#define		HUMID1_IN()				TestBit(HUMID1_PIN,	HUMID1)
#define		HUMID2_IN()				TestBit(HUMID2_PIN,	HUMID2)


#endif
#endif /* DHT11_H_ */

