/********************************************************************
		Created:	19-03-2014 08:17:26 PM
		Filename: 	GSM.c	
		Author:		Kaustubh P. Gabale, EmbedPro Solutions (+919028785320)

		Modified:	17 Jan, 2016: In function "Check_Rec_Data(..)" changed 
					timeout delay from 10ms to 1ms for greater resolution 
					for receiving USART data. Total effective timeout remains
					the same as 1sec.
					17 Jan, 2016: Added Calling Facility -> U8 Send_Call(unsigned char *mobNumCall)
					22 Jan, 2016: Added call reject or end -> U8 Reject_Call(void)
					12 May, 2016: Added Program Space Variables to avoid RAM problem.
					26 May, 2016: Added GPRS Functionality to configure as TCP/UDP Client, and send packet.
					14 Aug, 2017: Added "AT+CLIP=1" command for sending calling no. to MCU
                    17 Feb, 2018: Modified to use in PIC18 device
					04 Oct, 2018: Modified rxUartData == GSM_DEVICE for checking recd data
					16 Nov, 2019: Added local timestamp to receive => Enable_Local_Time() & Get_Local_Time(..)
								  added receiveCounterUART = 0; in function Check_Rec_Data(..) near return 0
					06 Dec, 2019: Added GSM_Check_Nw_Registration(): Check network registration status
								  Added U8 GSM_Send_Cmd_Pgm(const U8 *pgmCmd, const U8* chkData): Common funcction to
								  to send most commands.
*********************************************************************/

/********************************************************************
GSM

Gives the GSM Modem Communication facility with dedicated USART port.
Using this source code, one can initialize modem, send SMS, call to 
custom number, configure TCP/UDP client, send TCP/UDP packet to server.
Platform: ATmega8, ATmega16, ATmega32
********************************************************************/

#include "GSM.h"

#ifdef USE_GSM_MODULE

U8 cmdBuff[20];
const U8 *addr;
const U8 Cmd_AT[] PROGMEM = "AT";
const U8 Cmd_ATE0[] PROGMEM = "ATE0";
const U8 Cmd_CMGF[] PROGMEM = "AT+CMGF=1";
const U8 Cmd_CNMI[] PROGMEM = "AT+CNMI=1,2,0,0,0";
const U8 Cmd_R[] PROGMEM = "AT+CMGR=";
const U8 Cmd_S[] PROGMEM = "AT+CMGS=";
const U8 Cmd_D[] PROGMEM = "AT+CMGD=";
const U8 Cmd_Call[] PROGMEM = "ATD";
const U8 Cmd_Rejct_Call[] PROGMEM = "ATH";
const U8 Def_Mob_No1[] PROGMEM = "9028785320";
const U8 Cmd_MORING[] PROGMEM = "AT+MORING=1";
const U8 Cmd_CLIP[] PROGMEM = "AT+CLIP=1";
const U8 Cmd_TIMESTAMP[] PROGMEM = "AT+CLTS=1;&W";
const U8 Cmd_QueryTime[] PROGMEM = "AT+CCLK?";
const U8 Cmd_NwReg[] PROGMEM = "AT+CREG?";

const U8 Rply_MORING[] PROGMEM = "MO RING";
const U8 Rply_MOCONNCT[] PROGMEM = "MO CONNECTED";

const U8 Cmd_CGATT[] PROGMEM = "AT+CGATT?";
const U8 Cmd_CSTT[] PROGMEM = "AT+CSTT=\"CMNET\"";
const U8 Cmd_CIICR[] PROGMEM = "AT+CIICR";
const U8 Cmd_CIPMUX[] PROGMEM = "AT+CIPMUX=";
const U8 Cmd_CIFSR[] PROGMEM = "AT+CIFSR";
const U8 Cmd_CIPSTART[] PROGMEM = "AT+CIPSTART=";
const U8 Cmd_CIPSEND[] PROGMEM = "AT+CIPSEND";
const U8 GSM_TCP_str[] = "TCP";
const U8 GSM_UDP_str[] = "UDP";

const U8 GSM_End_Char[];
const U8 GSM_Eq_str = '=';
const U8 GSM_Quote_Str = '"';
const U8 GSM_Comma_Str = ',';
const U8 GSM_Double_Quote[] = {0x22, 0};			// Value for "
const U8 GSM_CTRL_Z[] = {0x1A, 0};					// Value for Ctrl+Z
const U8 GSM_OK[] = "OK";

//U8 Rec_GSM_Data = 0;
U8 Cmd_Sent = 0;

//extern U8 receiveBufferUART[], receiveCounterUART;

U8 Init_GSM(void)
{
	U8 OK1 = 0, OK2 = 0, OK3 = 0, OK4 = 0;

	if(Send_Cmd_AT() == GSM_CMD_SUCCESS)
	{
		OK2 = Send_Cmd_ATE0();

		if((OK2 == GSM_CMD_SUCCESS) && (OK3 == GSM_CMD_SUCCESS) && (OK4 == GSM_CMD_SUCCESS) && (OK1 == GSM_CMD_SUCCESS))
		{
			GSM_Enable_Local_Time();
			return GSM_CMD_SUCCESS;
		}
		
		else
		{
			
			return GSM_CMD_FAILED;
		}
	}

	else
	{
		
		return GSM_CMD_FAILED;
	}
}

U8 Send_Cmd_AT(void)
{
//	FlushRecBufferCommandResp();
	return(GSM_Send_Cmd_Pgm(Cmd_AT, 0, 0, (U8 *)GSM_OK));
}

U8 Send_Cmd_ATE0(void)
{
//	FlushRecBufferCommandResp();

	return(GSM_Send_Cmd_Pgm(Cmd_ATE0, 0, 0, (U8 *)GSM_OK));
}

U8 Send_Cmd_CMGF(void)
{
	return(GSM_Send_Cmd_Pgm(Cmd_CMGF, 0, 0, (U8 *)GSM_OK));
}

U8 Send_Cmd_CNMI(void)
{
	return(GSM_Send_Cmd_Pgm(Cmd_CNMI, 0, 0, (U8 *)GSM_OK));
}

U8 Send_Cmd_MORING(void)
{
	return(GSM_Send_Cmd_Pgm(Cmd_MORING, 0, 0, (U8 *)GSM_OK));
}

U8 Send_Cmd_CLIP(void)
{
	return(GSM_Send_Cmd_Pgm(Cmd_CLIP, 0, 0, (U8 *)GSM_OK));
}

U8 Send_SMS(unsigned char *msgText1, unsigned char *msgText2, unsigned char *mobNum)
{
	U8 Cursor = 0;

	Read_Pgm_String(Cmd_S, cmdBuff);//, 8);

	while(Initiate_UART_Transmission((U8 *)cmdBuff, 0) != TX_SUCCESS);					// Send AT+CMGS=
	while(Initiate_UART_Transmission((U8 *)GSM_Double_Quote, 1) != TX_SUCCESS);			// Send "
	
	if(mobNum == NULL)
	{
		mobNum = (U8 *)Def_Mob_No1;
	}

	while(Initiate_UART_Transmission((U8 *)mobNum, MOB_NO_LEN) != TX_SUCCESS);			// Send Receiver's mobile number
	while(Initiate_UART_Transmission((U8 *)GSM_Double_Quote, 1) != TX_SUCCESS);			// Send "
	
	//DIS_RX();
	_delay_ms(1);
	
	while(Initiate_UART_Transmission((U8 *)GSM_End_Char, 2) != TX_SUCCESS);					// Send 0x0D
	
	_delay_ms(500);
	
//	EN_RX();
	
	Cursor = 1;

	if(Cursor)
	{
		if(msgText1 != NULL)
		{
			while(Initiate_UART_Transmission((U8 *)&msgText1[0], strlen((const char *)msgText1)) != TX_SUCCESS);
		}

		if(msgText2 != NULL)
		{
			while(Initiate_UART_Transmission((U8 *)&msgText2[0], strlen((const char *)msgText2)) != TX_SUCCESS);
		}

		while(Initiate_UART_Transmission((U8 *)GSM_CTRL_Z, 1) != TX_SUCCESS);			// Send Ctrl+z

		Cmd_Sent = Check_Rec_Data((U8 *)GSM_OK);

		if(!Cmd_Sent)
		{
			Cmd_Sent = Check_Rec_Data((U8 *)"ERROR");
			return ~Cmd_Sent;				// Error in sending msg
		}

		return Cmd_Sent;
	}

	else
	{
		return GSM_CMD_FAILED;
	}
}

U8 Send_Call(unsigned char *mobNumCall)
{
	if(mobNumCall != NULL)
	{
		return(GSM_Send_Cmd_Pgm(Cmd_Call, mobNumCall, (U8 *)"i;", (U8 *)GSM_OK));
	}
	return GSM_CMD_FAILED;
}

U8 Reject_Call(void)
{
	return(GSM_Send_Cmd_Pgm(Cmd_Rejct_Call, 0, 0, 0));
}

U8 GPRS_Initialize(void)
{
	if(GSM_Send_Cmd_Pgm(Cmd_CGATT, 0, 0, (U8 *)"+CGATT:1") == GSM_CMD_FAILED)
	{
		return GSM_CMD_FAILED;
	}

	if(GSM_Send_Cmd_Pgm(Cmd_CSTT, 0, 0, (U8 *)GSM_OK) == GSM_CMD_FAILED)
	{
		return GSM_CMD_FAILED;
	}

	if(GSM_Send_Cmd_Pgm(Cmd_CIICR, 0, 0, (U8 *)GSM_OK) == GSM_CMD_FAILED)
	{
		return GSM_CMD_FAILED;
	}

	return GSM_CMD_SUCCESS;
}

U8 GPRS_Configure_Client(U8 numOfConn, U8 type, U8 *destIP, U8 *remotePort, U8 *localPort, U8 udpMode)
{
	Read_Pgm_String(Cmd_CIPMUX, cmdBuff);
	while(Initiate_UART_Transmission((U8 *)cmdBuff, TX_STR_TILL_NULL) != TX_SUCCESS);					// Send AT+CIPMUX=

	if(numOfConn >= '0')
	{
		Initiate_UART_Transmission((U8 *)"1", 1);                   // 1; Enable multiple connections
	}
	else
	{
		Initiate_UART_Transmission((U8 *)"0", 1);                   // 0; Enable single connection
	}

	while(Initiate_UART_Transmission((U8 *)GSM_End_Char, 2) != TX_SUCCESS);				// Send 0x0D

	if(Check_Rec_Data((U8 *)GSM_OK) != TX_SUCCESS)
	{
		return TX_ERROR;
	}

	Read_Pgm_String(Cmd_CIPSTART, cmdBuff);
	while(Initiate_UART_Transmission((U8 *)cmdBuff, TX_STR_TILL_NULL) != TX_SUCCESS);					// Send AT+CIPSTART=

	if(numOfConn >= '0')
	{
		Initiate_UART_Transmission((U8 *)&numOfConn, 1);                // Number of connection
		Initiate_UART_Transmission((U8 *)&GSM_Comma_Str, 1);            // ,
	}

	Initiate_UART_Transmission((U8 *)&GSM_Quote_Str, 1);                // "

	if(type == GSM_CONN_TCP)
	{
		Initiate_UART_Transmission((U8 *)GSM_TCP_str, TX_STR_TILL_NULL);    // TCP Connection
	}
	else if(type == GSM_CONN_UDP)
	{
		Initiate_UART_Transmission((U8 *)GSM_UDP_str, TX_STR_TILL_NULL);    // UDP Connection
	}
	
	Initiate_UART_Transmission((U8 *)&GSM_Quote_Str, 1);                    // "
	Initiate_UART_Transmission((U8 *)&GSM_Comma_Str, 1);                    // ,

	Initiate_UART_Transmission((U8 *)&GSM_Quote_Str, 1);                    // "
	Initiate_UART_Transmission((U8 *)destIP, TX_STR_TILL_NULL);             // Destination IP or Domain Name
	Initiate_UART_Transmission((U8 *)&GSM_Quote_Str, 1);                    // "
	Initiate_UART_Transmission((U8 *)&GSM_Comma_Str, 1);                    // ,

	Initiate_UART_Transmission((U8 *)remotePort, TX_STR_TILL_NULL);         // Remote Port

	Initiate_UART_Transmission((U8 *)GSM_End_Char, 2);   // 0x0d

	return (Check_Rec_Data((U8 *)"CONNECT OK"));
}

U8 GPRS_Send_Data(U8 numOfConn, U8 *sendData, U16 dataLen)
{
	Read_Pgm_String(Cmd_CIPSEND, cmdBuff);

	Initiate_UART_Transmission((U8 *)cmdBuff, TX_STR_TILL_NULL);     // AT+CIPSEND

	if(numOfConn >= '0')
	{
		Initiate_UART_Transmission((U8 *)&GSM_Eq_str, 1);					// Connection number
		Initiate_UART_Transmission((U8 *)&numOfConn, 1);                    // Connection number
	}
	
	receiveCounterUART = 0;
	Initiate_UART_Transmission((U8 *)GSM_End_Char, 2);		// 0x0D, 0x0A
	Initiate_UART_Transmission((U8 *)NULL, 0);				// Wait
	
	if(dataLen == TX_STR_TILL_NULL)
	{
		dataLen = strlen((const char*)sendData);
	}
	
	if(dataLen > 2048)
	{
		dataLen = 2048;
	}
	
	if(Check_Rec_Data((U8 *)">") == GSM_CMD_SUCCESS)
	{
		_delay_ms(10);
		Initiate_UART_Transmission((U8 *)sendData, dataLen);	// Send Data
		Initiate_UART_Transmission((U8 *)&GSM_CTRL_Z[0], 1);		// Send Ctrl+Z
		Initiate_UART_Transmission((U8 *)NULL, 0);				// Wait
	}
	else
	{
		return TX_ERROR;
	}
	
	return(Check_Rec_Data((U8 *)GSM_OK));
}

U8 GSM_Enable_Local_Time(void)
{
	return(GSM_Send_Cmd_Pgm(Cmd_TIMESTAMP, 0, 0, (U8 *)GSM_OK));
}

U8 GSM_Get_Local_Time(U8* timeBuff)
{
	const U8 timeHdr[] = "+CCLK:";
	char* strPtr = 0;
	
	if(timeBuff != NULL)
	{
		GSM_Send_Cmd_Pgm(Cmd_QueryTime, 0, 0, 0);
		if(Check_Rec_Data((U8 *)timeHdr))
		{
			_delay_ms(60);		// wait to receive full frame
			strPtr = strstr((const char *)receiveBufferUART, (const char *)"\"");
			if(strPtr != NULL)
			{
				strPtr ++;
				strlcpy((char*)&timeBuff[0], (const char *)strPtr, 18);		// Copy only time from rx data => +CCLK: "yy/MM/dd,hh:mm:ss+/-zz"     OK
				return GSM_CMD_SUCCESS;
			}
		}
	}
	return GSM_CMD_FAILED;
}

U8 GSM_Check_Nw_Registration(void)
{
	return(GSM_Send_Cmd_Pgm(Cmd_NwReg, 0, 0, (U8 *)"+CREG: 0,1"));
}

U8 GSM_Send_Cmd_Pgm(const U8 *pgmCmd1, const U8 *pgmCmd2, const U8 *pgmCmd3, const U8* chkData)
{
	if(pgmCmd1 != NULL)
	{
		Read_Pgm_String(pgmCmd1, cmdBuff);									// Cmd1
		Initiate_UART_Transmission((U8 *)cmdBuff, 0);	// Send cmd to uart
	}
	if(pgmCmd2 != NULL)
	{
		Read_Pgm_String(pgmCmd2, cmdBuff);									// Cmd2
		Initiate_UART_Transmission((U8 *)cmdBuff, 0);	// Send cmd to uart
	}
	if(pgmCmd3 != NULL)
	{
		Read_Pgm_String(pgmCmd3, cmdBuff);									// Cmd3
		Initiate_UART_Transmission((U8 *)cmdBuff, 0);	// Send cmd to uart
	}
	Initiate_UART_Transmission((U8 *)GSM_End_Char, 2);		// Send 0x0D
	
	if(chkData == 0)
	{
		return GSM_CMD_FAILED;
	}
	
	return(Check_Rec_Data((U8 *)chkData));
}

U8 Check_Rec_Data(unsigned char *Check_str)
{
	U8 *foundAddr = NULL;
	U16 TimeOut = 0;

	do
	{
//		if(rxUartData == GSM_DEVICE)
		if(Rec_GSM_Data == 1)
		{
			if(receiveCounterUART >= strlen((const char *)Check_str))
			{
				foundAddr = (U8*)strstr((const char *)&receiveBufferUART[0], (const char *)Check_str);
				if(foundAddr != NULL)
				{
					receiveCounterUART = 0;
					//rxUartData = IDLE_DEVICE;
					Rec_GSM_Data = 0;
					return 1;
				}
			}
		}
		_delay_ms(1);
		TimeOut++;
	}
	while(TimeOut <= 1500);

	receiveCounterUART = 0;
	return 0;
}

#endif // USE_GSM_MODULE
