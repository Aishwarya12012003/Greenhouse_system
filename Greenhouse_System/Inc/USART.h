/*
 * USART.h
 *
 * Created: 19-03-2014 08:17:40 PM
 *  Author: Koustubh Gabale
 */ 


#ifndef USART_H_
#define USART_H_

#include <string.h>
#include "HardwareProfile.h"

extern U8 receiveBufferUART0[], receiveBufferUART1[];
extern U16 receiveCounterUART0, receiveCounterUART1;

void USART0Initialize(U16 baudVal);
void USART1Initialize(U16 baudVal);
//U8 Initiate_UART_Transmission(U8 *txBuffInit, U8 txL);
void Prepare_Tx_Frame(U8 *txBuffer, U8 txLen);
void FlushRecBuffer(void);
int sendCharU0(int ch);
int sendStrU0(U8* str);
int sendStrLenU0(U8* str, unsigned char len);
int sendCharU1(int ch);
int sendStrU1(U8* str);
int sendStrLenU1(U8* str, unsigned char len);
/*__irq*/ void ISRSerialTransmit(void);
void UART0RX_VectoredIRQ (void)  __irq;
void ISRSerial0Receive(void);
void UART1RX_VectoredIRQ (void)  __irq;
void ISRSerial1Receive(void);

#define  RX_FRAME_LENGTH        receiveBufferUART[1]
#define  TX_FRAME_LENGTH        txFrameLength//transmitBufferUART[1]

#define  MAX_TRANSMIT_LENGTH    100
#define  MAX_RECEIVE_LENGTH     300
#define  MAX_CMD_RESP_LENGTH    10
#define  MAX_OTHR_DATA_LENGTH   20

#define TX_SUCCESS              1
#define TX_ERROR                0

#define	XBEE_DEVICE             0x14
#define	GSM_DEVICE             	0x15
#define	ESP_DEVICE             	0x16
#define	PC_DEVICE             	0x17
#define IDLE_DEVICE							0

#define	FRAME_START_DELIMITER		'<'
#define	FRAME_END_DELIMITER     '>'
#define	FRAME_SEPARATOR         ':'

#define	START_FRAME_REC       0x01
#define	FRAME_RECD            0x02
#define	GSM_DATA_REC         	0x03
#define	OTHER_DATA_REC        0x04
#define	ESP_DATA_REC         	0x05

#define	TX_STR_TILL_NULL		0

#define TX0_REGISTER			U0THR
#define RX0_REGISTER			U0RBR
#define BAUD0_REG					U0DLL

#define TX1_REGISTER			U1THR
#define RX1_REGISTER			U1RBR
#define BAUD1_REG					U1DLL

#define TX_FLAG						TXC
#define RX_FLAG						RXC
#define REG_EMPTY_FLAG		UDRE

#define SET_TX_INT_FLAG()		SetBit(UCSRA, TX_FLAG)

#define CHECK_RX_COMPLETE()		TestBit(UCSRA, RX_FLAG)
#define CHECK_TX_COMPLETE()		TestBit(UCSRA, TX_FLAG)
#define CHECK_REG_EMPTY()		TestBit(UCSRA, REG_EMPTY_FLAG)

#define EN_RX()					SetBit(UCSRB, RXEN)
#define EN_TX()					SetBit(UCSRB, TXEN)
#define DIS_RX()				ClearBit(UCSRB, RXEN)
#define DIS_TX()				ClearBit(UCSRB, TXEN)

#define EN_RX_INT()				SetBit(UCSRB, RXCIE)
#define EN_TX_INT()				SetBit(UCSRB, TXCIE)
#define EN_REG_EMPTY_INT()		SetBit(UCSRB, UDRIE)

#define EN_RX_ONLY				0x10
#define EN_TX_ONLY				0x08
#define EN_RX_TX				0x18

#define BAUDRATE 9600

#if (F_CPU == 11059200)
/* Standard Baudrate Values for UBRR  @ Fosc = 11.0592MHz */
#define BAUDRATE_115200 5
#define BAUDRATE_76800  8
#define BAUDRATE_57600  11
#define BAUDRATE_38400  17
#define BAUDRATE_28800  23
#define BAUDRATE_19200  35
#define BAUDRATE_14400  47
#define BAUDRATE_9600   71
#define BAUDRATE_4800   143
#define BAUDRATE_2400   287

#elif (F_CPU == 60000000)		// PCLK same as FOSC

#define BAUDRATE_115200		32 
#define BAUDRATE_76800  
#define BAUDRATE_57600  
#define BAUDRATE_38400  
#define BAUDRATE_28800  
#define BAUDRATE_19200  
#define BAUDRATE_14400  
#define BAUDRATE_9600		395
#define BAUDRATE_4800   
#define BAUDRATE_2400   

#else
#error "Non Standard Crystal Value. Standard Value: 11.0592 MHz. Define in HardwareProfile.h"
#endif

#endif /* USART_H_ */
