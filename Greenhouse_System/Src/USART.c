/********************************************************************
		Created:	19-03-2014 08:17:26 PM
		Modified:	-
		Filename: 	USART.c	
	

*********************************************************************/

/********************************************************************
USART

Gives the USART Communication facility with dedicated USART port.
Platform: ARM7 LPC2148
********************************************************************/
#ifdef Green_Solutions

#include "USART.h"

U8 txFrameLength;
U8 enableTx = 0;

U16 receiveCounterUART0 = 0, transmitCounterUART0 = 0;
U16 receiveCounterUART1 = 0, transmitCounterUART1 = 0;
U8 receiveBufferUART0[MAX_RECEIVE_LENGTH], transmitBufferUART0[MAX_TRANSMIT_LENGTH], receiveOthrDataBuff0[MAX_OTHR_DATA_LENGTH];
U8 receiveBufferUART1[MAX_RECEIVE_LENGTH], transmitBufferUART1[MAX_TRANSMIT_LENGTH], receiveOthrDataBuff1[MAX_OTHR_DATA_LENGTH];
U8 curSerialDevice = GSM_DEVICE;
U8 enableRecMsg0 = 0, enableRecMsg1 = 0;


/************************************************************************
 Function Name      : void USARTInitialize(U16 baudVal, U8 rxTxEnable)
 Description        : Initializes USART with user selected Baudrate & Receiver/Transmitter Enable
 					   					  Values define in USART.h
 Parameters Returned: None
*************************************************************************/

/********************* Formula for Baud Rate ***********************
BAUD = PCLK / (16 * (256 * U0DLM + U0DLL) + (1 + DivAddVal / MulVal)
*******************************************************************/

void USART0Initialize(U16 baudVal)
{
	U0LCR = 0x83;             	// Enable DLAB, 8 bits, no Parity, 1 Stop bit

	U0DLL = baudVal & 0xFF;		// Set Baud Rate
  	U0DLM = baudVal >> 8;

//	U0FDR = 0x85;				// DivVal = 5, MulVal = 8

 	U0LCR = 0x03;             	// Disable DLAB, 8-bit, No Parity, , 1 Stop bit
	U0IER = 0x01;				//Receive Data Available Interrupt Enable

	VICVectCntl6 	= 0x20 | 6;  						//select a priority slot for a UART0 interrupt
	VICVectAddr6 	= (unsigned)UART0RX_VectoredIRQ;	//pass the address of the IRQ into the VIC slot
	SetBit(VICIntEnable, 6);							//Enable the UART0 interrupt
}

void USART1Initialize(U16 baudVal)
{
	U1LCR = 0x83;             	// Enable DLAB, 8 bits, no Parity, 1 Stop bit

	U1DLL = baudVal & 0xFF; 	// Set Baud Rate
  	U1DLM = baudVal >> 8;

//	U1FDR = 0x85;				// DivVal = 5, MulVal = 8

 	U1LCR = 0x03;             	// 8-bit, No Parity
	U1IER = 0x01;				//Receive Data Available Interrupt Enable

	VICVectCntl7 	= 0x20 | 7;  						//select a priority slot for a UART1 interrupt
	VICVectAddr7 	= (unsigned)UART1RX_VectoredIRQ;	//pass the address of the IRQ into the VIC slot
	SetBit(VICIntEnable, 7);							//Enable the UART1 interrupt
}

/*************************************************************************
 * Function name       : U8 Initiate_UART_Transmission(U8 *txBuffInit, U8 txL)
 *
 * Description         : Initiates the USART transmission by enabling USART transmitter.
 *                      When USART transmitter is enabled, TX Flag is set & hence, interrupt
 *                      is occurred. Further in ISR the frame of given length is transmitted.
 *
 * Parameters Passed   : None
 *
 * Parameters Returned : U8 - Transmission Status
 *                      Returns 1 if transmission started & 0 if USART is busy in other task
 *
 * Dependencies        : None
*************************************************************************/
//U8 Initiate_UART_Transmission(U8 *txBuffInit, U8 txL)
//{
////	U8 txStatus = TX_ERROR;
//	
//	/* Check if USART is not busy in transmission */
//	if(transmitCounterUART1 == 0)
//	{
//		if(txBuffInit != NULL)
//		{
//			if(txL == TX_STR_TILL_NULL)
//			{
//				txL = strlen((const char*)txBuffInit);
//			
//			}

//			sendStrLenU1(txBuffInit, txL);
//			//Prepare_Tx_Frame(txBuffInit, txL);
//			return TX_SUCCESS;                       	/* Transmission Started in ISR */
//		}
//		//SET_TX_INT_FLAG();                  			/* Set Transmit Interrupt flag to send first char of UART  in its ISR  */
//		//TX_REGISTER = 0x00;
//		return TX_SUCCESS;                       	/* Transmission Started in ISR */

//	}

//	return TX_ERROR;							/* Transmission could not start */
//}

/*************************************************************************
 * Function name       : void Prepare_Tx_Frame(U8 *txBuffer, U8 txLen)
 *
 * Description         : Prepares frame to be transmitted through USART.
 *
 * Parameters Passed   : None
 *
 * Parameters Returned : None
 *
 * Dependencies        : None
*************************************************************************/
/*void Prepare_Tx_Frame(U8 *txBuffer, U8 txLen)
{
	U8 cnt;
	if(txLen != 0)
	{
		txFrameLength = txLen;											// Length from User
	}
	
	else
	{
		txFrameLength = txBuffer[1];			//Length from Buffer 2nd byte
		txLen = txBuffer[1];
	}
	
	for(cnt = 0; cnt < txLen; cnt++)
	{
		transmitBufferUART[cnt] = txBuffer[cnt];
	}
	
	transmitCounterUART = 0;
	enableTx = 1;
//	TX_REGISTER = transmitBufferUART[0];
}
*/

void FlushRecBuffer0(void)				// Clear only first 10B to clear prev command response characters
{
	U16 Rec_Count;
	
	for(Rec_Count = 0; Rec_Count < MAX_RECEIVE_LENGTH ; Rec_Count++)
		receiveBufferUART0[Rec_Count] = 0;

	receiveCounterUART0 = 0;
	Rec_Count = 0;
}

void FlushRecBuffer1(void)				// Clear only first 10B to clear prev command response characters
{
	U16 Rec_Count;
	
	for(Rec_Count = 0; Rec_Count < MAX_RECEIVE_LENGTH ; Rec_Count++)
		receiveBufferUART1[Rec_Count] = 0;

	receiveCounterUART0 = 0;
	Rec_Count = 0;
}

void FlushTransBuffer0(void)				// Clear only first 10B to clear prev command response characters
{
	U16 Rec_Count;
	
	for(Rec_Count = 0; Rec_Count < MAX_TRANSMIT_LENGTH ; Rec_Count++)
		transmitBufferUART0[Rec_Count] = 0;

	transmitCounterUART0 = 0;
	Rec_Count = 0;
}

int sendCharU0(int ch)
{                 /* Write character to Serial Port    */

  while (!(U0LSR & 0x20));
	U0THR = ch;
  return (1);
}

int sendStrU0(U8* str)
{
	while(*str)
	{
		while (!(U0LSR & 0x20));
		U0THR = *str;
		str++;
	}
	return (1);
}

int sendStrLenU0(U8* str, unsigned char len)
{
	while(len)
	{
		while (!(U0LSR & 0x20));
		U0THR = *str;
		str++;
		len--;
	}
	return (1);
}

int sendCharU1(int ch)
{                 /* Write character to Serial Port    */

  while (!(U1LSR & 0x20));
	U1THR = ch;
  return (1);
}

int sendStrU1(U8* str)
{
	while(*str)
	{
		while (!(U1LSR & 0x20));
		U1THR = *str;
		str++;
	}
	return (1);
}

int sendStrLenU1(U8* str, unsigned char len)
{
	while(len)
	{
		while (!(U1LSR & 0x20));
		U1THR = *str;
		str++;
		len--;	
		
	}
	return (1);
}
/*************************************************************************
 * Function name       : void ISRSerialTransmit(void)
 *
 * Description         : Interrupt Service Routine for USART Transmission. This function
 *                      should not be called anywhere other than Interrupt Handler Routine.
 *                      This function transmits data frame of a variable frame length whose
 *                      frame length is stored into buffer. The frame to be transmitted is
 *                      stored into "transmitBufferUART".
 *
 * Parameters Passed   : None
 *
 * Parameters Returned : None
 *
 * Dependencies        : None
*************************************************************************/
/*void ISRSerialTransmit(void)
{
	if(enableTx == 1)
	{
			if(transmitCounterUART <  TX_FRAME_LENGTH)
			{
					sendCharU0(transmitBufferUART[transmitCounterUART]);
//					TX_REGISTER = transmitBufferUART[transmitCounterUART];
					transmitCounterUART++;
			}

			else if(transmitCounterUART >= TX_FRAME_LENGTH)
			{
					transmitCounterUART = 0;
					enableTx = 0;
			}
	}
}*/

void UART0RX_VectoredIRQ (void)  __irq
{
	ISRSerial0Receive();
	VICVectAddr = 0;
}

void UART1RX_VectoredIRQ (void)  __irq
{
	ISRSerial1Receive();
	VICVectAddr = 0;
}
#endif	
