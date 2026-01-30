#ifndef GSM_H
#define GSM_H


#include <string.h>
#include "HardwareProfile.h"
#include "USART.h"
#if !defined (__XC8)
//#include "ProgMemRead.h"
#endif

#ifdef USE_GSM_MODULE

U8 Init_GSM(void);
U8 Send_Cmd_AT(void);
U8 Send_Cmd_ATE0(void);
U8 Send_Cmd_CMGF(void);
U8 Send_Cmd_CNMI(void);
U8 Send_Cmd_MORING(void);
U8 Send_Cmd_CLIP(void);
U8 Send_SMS(U8 *msgText1, U8 *msgText2, U8 *mobNum);
U8 Send_Call(unsigned char *mobNumCall);
U8 Reject_Call(void);
U8 GPRS_Initialize(void);
U8 GPRS_Configure_Client(U8 numOfConn, U8 type, U8 *destIP, U8 *remotePort, U8 *localPort, U8 udpMode);
U8 GPRS_Send_Data(U8 numOfConn, U8 *sendData, U16 dataLen);
U8 GSM_Enable_Local_Time(void);
U8 GSM_Get_Local_Time(U8* timeBuff);
U8 GSM_Check_Nw_Registration(void);
U8 GSM_Send_Cmd_Pgm(const U8 *pgmCmd1, const U8 *pgmCmd2, const U8 *pgmCmd3, const U8* chkData);
U8 Check_Rec_Data(unsigned char *Check_str);


#define		GSM_CMD_SUCCESS		1
#define		GSM_CMD_FAILED		0

#define		CLEAR_CNT			0
#define		NO_CLEAR_CNT		1

#define		SMS_REC				"+CMT"
#define		CALL_REC			"+CLIP"

#define		MOB_NO_LEN			10			// 10 digit mob no.

#define     GSM_CONN_TCP            'T'
#define     GSM_CONN_UDP            'U'


#if defined (__XC8)
#define     Read_Pgm_String(str1, str2)     strcpy((char*)str2, (const char*)str1)
#endif  // __XC8
#define     Read_Pgm_String(str1, str2)     strcpy((char*)str2, (const char*)str1)

#endif // USE_GSM_MODULE

#endif	// GSM_H
