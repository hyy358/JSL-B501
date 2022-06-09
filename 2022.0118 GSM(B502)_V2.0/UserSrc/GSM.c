#include "Driver.h"
#include "stdio.h"
#include "UART.h"
#include "GSM.h"
#include "SysStatus.h"
#include "SysString.h"

#define GSM_4G

#define AT_CMD_END "\x0d\x0a"
#define AT_CMD_CR  '\x0d'
#define AT_CMD_LF  '\x0a'
#define AT_CMD_CTRL_Z "\x1a"

GsmStatusType GsmStatus = GSM_ST_UNKNOW;
unsigned int GsmInitTime;
uint8_t GsmCsq = 0xff;
bool GsmCsqFlag = false;
bool GsmCallOk = false;
bool WaitCsq = false;
AtCmdRspType AtCmdCb_at(uint8_t* pRspStr)
{
//    AtCmdRspType  rspValue = AT_RSP_WAIT;
//    int  rspType = -1;
//    uint8_t  i = 0;
    uint8_t  *p = pRspStr;
    static uint8_t count = 0;
//    while(p) {
        /* ignore \r \n */
//        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
//        {
//            p++;
//        }
        if(lookforstr(p,"OK")!=-1)
		{
			return AT_RSP_FUN_OVER;
		}
//        p = (uint8_t*)strchr(p,0x0a);
//    }
	if(++count >= 10)
	{
		count = 0;
		return AT_RSP_ERROR;
	}
    return AT_RSP_WAIT;
}

AtCmdRspType AtCmdCb_cpin(uint8_t* pRspStr)
{
    AtCmdRspType  rspValue = AT_RSP_WAIT;
    code uint8_t *rspStrTable[] = {"+CPIN: NOT INSERTED","ERROR","+CPIN: READY","+CPIN: NOT READY"};
    int  rspType = -1;
    uint8_t  n = 0;
    uint8_t  stat = 0;
    uint8_t  i = 0;
    uint8_t  *p = pRspStr;
    static uint8_t count = 0;
//    while(p) {
//        /* ignore \r \n */
//        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
//        {
//            p++;
//        }

        for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
			if(lookforstr(p,rspStrTable[i])!=-1)
            {
                rspType = i;
                if (rspType == 1){
                    sscanf(p+strlen(rspStrTable[rspType]),"%d,%d",&n,&stat);
                }
                break;
            }
        }
//        p = (u8*)strchr(p,0x0a);
//    }

    switch (rspType)
    {
		case 0:  /* NOT INSERTED */
			rspValue = AT_RSP_ERROR;
			GsmCsq = 0xFE;
//			ShowGsmCsq();
			break;
		case 3:  /* NOT READY */
			if (count >= 10)
			{
				rspValue = AT_RSP_ERROR;
				count = 0;
			}
            else {
                rspValue  = AT_RSP_WAIT;
                count++;
            }
			GsmCsq = 0xFD;
//			ShowGsmCsq();
			break;
        case 1:  /* ERROR */
			if (count >= 10)
			{
				rspValue = AT_RSP_ERROR;
				count = 0;
			}
            else {
                rspValue  = AT_RSP_WAIT;
                count++;
            }
        break;

        case 2:  /* +CPIN */
			GsmCsq = 0xFC;
//			ShowGsmCsq();
			rspValue  = AT_RSP_FUN_OVER;
			break;
        default:
			break;
    }

    printf("at+cpin? return %bd\r\n",rspValue);
    return rspValue;
}

AtCmdRspType AtCmdCb_creg(uint8_t* pRspStr)
{
    AtCmdRspType  rspValue = AT_RSP_WAIT;
    code uint8_t *rspStrTable[] = {"ERROR","+CREG: "};
    int  rspType = -1;
    uint8_t  n = 0;
    uint8_t  stat = 0;
    uint8_t  i = 0;
    uint8_t  *p = pRspStr;
	int  tmp;
    static uint8_t count = 0;
//    while(p) {
//        /* ignore \r \n */
//        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
//        {
//            p++;
//        }

        for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
			tmp = lookforstr(p,rspStrTable[i]);
			if(tmp != -1)
            {
				p += tmp;
                rspType = i;
                if (rspType == 1){
                    sscanf(p+strlen(rspStrTable[rspType]),"%bd,%bd",&n,&stat);
					printf("AtCmdCb_creg: %bd,%bd\r\n",n,stat);
                }
                break;
            }
        }
//        p = (u8*)strchr(p,0x0a);
//    }

    switch (rspType)
    {

        case 0:  /* ERROR */
			if (count >= 20){
                rspValue = AT_RSP_ERROR;
				count = 0;
            }
            else {
                rspValue  = AT_RSP_WAIT;
                count++;
            }
        break;

        case 1:  /* +CREG */
			if(1 == stat)
			{                          /* registered */
				rspValue  = AT_RSP_FUN_OVER;
				count = 0;
			}
			else {                     /* searching */
				if (count >= 20){
//				#ifdef GSM_4G
//					rspValue = AT_RSP_FUN_OVER;
//				#else
					rspValue = AT_RSP_ERROR;
//				#endif
					
					count = 0;
				}
				else {
					rspValue  = AT_RSP_WAIT;
					count++;
				}
			}
        break;

        default:
        break;
    }

    printf("at+creg? return %bd\r\n",rspValue);

    return rspValue;
}
AtCmdRspType AtCmdCb_csq(uint8_t* pRspStr)
{
    AtCmdRspType  rspValue = AT_RSP_WAIT;
    code uint8_t *rspStrTable[] = {"ERROR","+CSQ: "};
    int  rspType = -1;
    uint8_t  csq = 0;
	uint8_t  stat = 0;
    uint8_t  i = 0;
    uint8_t  *p = pRspStr;
	int  tmp;
    static uint8_t count = 0;
	
	WaitCsq = false;
//    while(p) {
//        /* ignore \r \n */
//        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
//        {
//            p++;
//        }

        for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
			tmp = lookforstr(p,rspStrTable[i]);
			if(tmp != -1)
            {
				p += tmp;
                rspType = i;
                if (rspType == 1){
                    sscanf(p+strlen(rspStrTable[rspType]),"%bd,%bd",&csq,&stat);
					printf("AtCmdCb_csq: %bd,%bd\r\n",csq,stat);
                }
                break;
            }
        }
//        p = (u8*)strchr(p,0x0a);
//    }

    switch (rspType)
    {

        case 0:  /* ERROR */
			if (count == 10){
                rspValue = AT_RSP_ERROR;
				count = 0;
            }
            else {
                rspValue  = AT_RSP_WAIT;
                count++;
            }
        break;

        case 1:  /* +CREG */
			GsmCsq = csq;
			GsmCsqFlag = true;
			rspValue  = AT_RSP_FUN_OVER;
        break;

        default:
        break;
    }

    printf("at+csq? return %bd\r\n",rspValue);

    return rspValue;
}

AtCmdRspType AtCmdCb_clcc(uint8_t* pRspStr)
{
//    AtCmdRspType  rspValue = AT_RSP_WAIT;
//    int  rspType = -1;
//    uint8_t  i = 0;
    uint8_t  *p = pRspStr;
    static uint8_t count = 0;
	if(lookforstr(p,"OK")!=-1)
	{
		return AT_RSP_FUN_OVER;
	}
	if(++count >= 10)
	{
		count = 0;
		return AT_RSP_ERROR;
	}
    return AT_RSP_WAIT;
}
uint8_t AtCmdIdx = 0xFF;
uint8_t AtCmdRetryTime = 0;

#ifdef GSM_4G
#define INIT_TIMER 2000
#define GSM_AT_COUNT 13
AtCmdEntity AtCmdBuf[GSM_AT_COUNT]={
        {"AT\r\n",AtCmdCb_at},
		{"AT+CPIN?\r\n",AtCmdCb_cpin},
		//{"AT+COPS?\r\n",AtCmdCb_at},
		{"AT+COPS=0\r\n",AtCmdCb_at},
		{"AT+QCFG=\"nwscanmode\",0\r\n",AtCmdCb_at},
		{"AT+CREG?\r\n",AtCmdCb_creg},
		{"AT+CFUN=1\r\n",AtCmdCb_at},
//		{"AT+CLCC=1\r\n",AtCmdCb_clcc},
		{"AT+QURCCFG=\"urcport\",\"uart1\"\r\n",AtCmdCb_at},
		{"AT+CTZU=1\r\n",AtCmdCb_at},
//		{"AT+CSDVC=1\r\n",AtCmdCb_at},
		{"AT+CLIP=1\r\n",AtCmdCb_at},
//		{"AT+CLCC=1\r\n",AtCmdCb_clcc},
		//{"AT+DDET=1,0,0\r\n",AtCmdCb_at},
		{"AT+CGMR\r\n",AtCmdCb_at},
		{"AT+QTONEDET=1\r\n",AtCmdCb_at},
//		{"AT+VTD=6\r\n",AtCmdCb_at},
		{"AT+CLVL=4\r\n",AtCmdCb_at},
		{"AT+CSQ\r\n",AtCmdCb_csq},
};
#else
#define INIT_TIMER 1000
#define GSM_AT_COUNT 7
AtCmdEntity AtCmdBuf[GSM_AT_COUNT]={
        {"AT\r\n",AtCmdCb_at},
		{"AT+CPIN?\r\n",AtCmdCb_cpin},
		{"AT+CREG?\r\n",AtCmdCb_creg},
		{"AT+CLCC=1\r\n",AtCmdCb_clcc},
		{"AT+DDET=1,0,0\r\n",AtCmdCb_at},
		{"AT+CLVL=90\r\n",AtCmdCb_at},
		{"AT+CSQ\r\n",AtCmdCb_csq},
};
#endif
uint8_t StrSetVolum[20];
void GSM_Repower(void)
{
	PrintString3("AT+CPOF\r\n");
	GsmStatus = GSM_ST_PWRDOWN;
	GsmInitTime = 2000;
	
//	GsmCsq = 0xff;
	GsmCsqFlag = true;
	
	printf("GSM_Repower\r\n");
}
void GSM_StartInit(void)
{
	GSM_PWREN_H;
	GsmInitTime = 1000;
	GsmStatus = GSM_ST_KEYH;
	
	GsmCsq = 0xff;
	GsmCsqFlag = true;
	printf("GSM_Init:1\r\n");
}

void GSM_GetCsq(void)
{
	if(GsmStatus == GSM_ST_CALLIN)
	{
		return;
	}
	GsmInitTime = 1000;
	GsmStatus = GSM_ST_WAITAT;
	AtCmdIdx = GSM_AT_COUNT-1;
	WaitCsq = true;
	printf("GSM_GetCsq\r\n");
}
void GSM_SetVolum(uint8_t volum)
{
	GsmInitTime = 1000;
	GsmStatus = GSM_ST_WAITAT;
	AtCmdIdx = GSM_AT_COUNT-2;
#ifdef GSM_4G
	sprintf(StrSetVolum,"AT+CLVL=%bd\r\n",volum);
#else
	sprintf(StrSetVolum,"AT+CLVL=%bd\r\n",(volum*20)-1);
#endif
	
	AtCmdBuf[AtCmdIdx].p_atCmdStr = StrSetVolum;
	printf("GSM_SetVolum (%bd): %s\r\n",volum,AtCmdBuf[AtCmdIdx].p_atCmdStr);
}
uint16_t CallOutTime = 0;
uint16_t CallInTime = 0;
uint16_t TalkTime = 0;
uint8_t PaOpenTime = 0;
bool GSM_Call(uint8_t * tel)
{
	if((GsmStatus == GSM_ST_ATOVER) || (GsmStatus == GSM_ST_WAITHUNGUP) || (WaitCsq))
	{
		PrintString3("ATD");
		PrintString3(tel);
		PrintString3(";\r\n");
		CallOutTime = SysPara.RingOutTs;
		GsmInitTime = 1000;
		GsmStatus = GSM_ST_CALL;
		GsmCallOk = false;
		//PA_PWR_L;
		//PaOpenTime = 1;
		PaOpenTime = 0;
		PA_PWR_H;
		return true;
	}
	return false;
}
void GSM_Huangup(void)
{
	PA_PWR_H;
//#ifdef GSM_4G
	PrintString3("AT+CHUP\r\n");
//#else
//	PrintString3("ATH\r\n");
//#endif
	
	GsmStatus = GSM_ST_ATOVER;
	TalkTime = 0;
}

void GSM_InitTask(void)
{
	switch(GsmStatus)
	{
		case GSM_ST_PWRDOWN:
			if(--GsmInitTime == 0)
			{
				GSM_PWREN_L;
				GsmInitTime = 15000;
				GsmStatus = GSM_ST_REPWR;
				AtCmdRetryTime = 0;
			}
			break;
		case GSM_ST_REPWR:
			if(--GsmInitTime == 0)
			{
				GSM_StartInit();
			}
			break;
		case GSM_ST_KEYH:
			if(--GsmInitTime == 0)
			{
				GSM_PWRKEY_H;
				GsmInitTime = 1200;
				GsmStatus = GSM_ST_KEYL;
				printf("GSM_Init:2\r\n");
			}
			break;
		case GSM_ST_KEYL:
			if(--GsmInitTime == 0)
			{
				GSM_PWRKEY_L;
				GsmInitTime = 600;
				GsmStatus = GSM_ST_AT;
				printf("GSM_Init:3\r\n");
			}
			break;
		case GSM_ST_AT:
			if(--GsmInitTime == 0)
			{
				//PrintString3(GsmATcmd[0]);
				GsmStatus = GSM_ST_WAITAT;
				AtCmdIdx = 0;
				AtCmdRetryTime = 0;
				
			#ifdef GSM_4G
				sprintf(StrSetVolum,"AT+CLVL=%bd\r\n",SysPara.TalkVolum);
			#else
				sprintf(StrSetVolum,"AT+CLVL=%bd\r\n",(SysPara.TalkVolum*20)-1);
			#endif
				AtCmdBuf[GSM_AT_COUNT-2].p_atCmdStr = StrSetVolum;
				
				PrintString3(AtCmdBuf[AtCmdIdx].p_atCmdStr);
				GsmInitTime = INIT_TIMER;
				printf("GSM_Init:4\r\n");
			}
			break;
		case GSM_ST_WAITAT:
			if(--GsmInitTime == 0)
			{
				if(++AtCmdRetryTime < 21)
				{
					PrintString3(AtCmdBuf[AtCmdIdx].p_atCmdStr);
					GsmInitTime = INIT_TIMER;
					printf("GSM_AtCmd:%bd - %bd\r\n",AtCmdIdx,AtCmdRetryTime);
				}
				else
				{
					printf("AT cmd execute 10ed Fail : %bd [%s] !\r\n",AtCmdIdx,AtCmdBuf[AtCmdIdx].p_atCmdStr);
					GSM_Repower();
				}
			}
			break;
		case GSM_ST_ATOVER:
			if(--GsmInitTime == 0)
			{
				GSM_GetCsq();
			}
			break;
		case GSM_ST_CALL:
			if(--GsmInitTime == 0)
			{
				GsmInitTime = 1000;
				printf("CallOutTime: %d\r\n",CallOutTime);
				if(--CallOutTime == 0)
				{
					GSM_Huangup();	
					TalkTime = 0;
//					while(CallTleIdx < (TEL_COUNT-1))
//					{
//						CallTleIdx++;
//						if(CallByNameItem(&NameItemCall,CallTleIdx))
//						{
//							SysStatus = SYS_CALL;
//							return;
//						}
//					}
					if(strlen(NameItemCall.Tel[CallTleIdx+1]) > 0)
					{
						GsmStatus = GSM_ST_WAITHUNGUP;
						CallOutTime = 2;
						return;
					}
					
					ShowErr();
					IdleTime = 5;
				}
				//else if(GsmCallOk == false)
				{
					PrintString3("AT+CLCC\r\n");
					//PrintString3("AT+CPAS\r\n");
					//PrintString3("AT+COLP?\r\n");
				}
			}
			break;
		case GSM_ST_CALLIN:
			if(--GsmInitTime == 0)
			{
				GsmInitTime = 1000;
				if(--CallInTime == 0)
				{
					PrintString3("ATA\r\n");
					SysStatus = SYS_TALK;
					GsmStatus = GSM_ST_TALK;
					//CallOutTime = SysPara.TalkTime;
					PA_PWR_L;
					
					TalkTime = SysPara.TalkTime;
					Volum = SysPara.TalkVolum;
					GSM_SetVolum(Volum);
				}
				else
				{
					//PrintString3("AT+CLCC\r\n");
					//PrintString3("AT+CPAS\r\n");
					//PrintString3("AT+COLP?\r\n");
				}
			}
			break;
		case GSM_ST_TALK:
			if(--GsmInitTime == 0)
			{
				GsmInitTime = 1000;
//				if(--CallOutTime == 0)
//				{
//					GSM_Huangup();
//					SysIdleCon();
//				}
			}
			break;
		case GSM_ST_WAITHUNGUP:
			if(--GsmInitTime == 0)
			{
				GsmInitTime = 1000;
				if(--CallOutTime == 0)
				{
					while(CallTleIdx < (TEL_COUNT-1))
					{
						CallTleIdx++;
						if(CallByNameItem(&NameItemCall,CallTleIdx))
						{
							SysStatus = SYS_CALL;
							return;
						}
					}
					GSM_Huangup();
					SysIdleCon();
				}
			}
			break;
		default:
			break;
	}
}

void ModemReceiveHandle(uint8_t *buf)
{
	code char *rspStrTable[] = {
		"NORMAL POWER DOWN",
		"UNDER-VOLTAGE POWER DOWN",
		"NO DIALTONE",
		"NO CARRIER",
		"RING",
		"+CLCC: ",
		"+DTMF: ",
		"+RXDTMF: ",
		"+COLP:",
		"+QTONEDET:"};
	int  rspType = -1;
    int  i = 0;
    uint8_t  *p = buf;
	uint8_t j,stat;

	for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
	{
		if(lookforstr(p,rspStrTable[i])!=-1)
		{
				rspType = i;
				break;
		}
	}
	switch(rspType)
	{
		case 0:
		case 1:
		case 2:
			if((GsmStatus == GSM_ST_CALL) || (GsmStatus == GSM_ST_TALK) || (GsmStatus == GSM_ST_WAITHUNGUP))
			{
//				PA_PWR_H;
				ShowGsmErr();
				IdleTime = 5;
			}
			GSM_Repower();
			break;
		case 3://对方挂机
			//PrintString3("ATH\r\n");
			if((GsmStatus == GSM_ST_CALL) && (strlen(NameItemCall.Tel[CallTleIdx+1]) > 0))
			{
				GsmStatus = GSM_ST_WAITHUNGUP;
				CallOutTime = 2;
				return;
			}
			if(GsmStatus == GSM_ST_WAITHUNGUP)
			{
				return;
			}
			GSM_Huangup();
			SysIdleCon();
			break;
		case 4://振铃
			//PA_PWR_L;
			//PrintString3("ATA\r\n");
			if(GsmStatus != GSM_ST_CALLIN)
			{
				GsmStatus = GSM_ST_CALLIN;
				//CallInTime = SysPara.RingInTs;
				if(SysPara.RingInTs == 1)
				{
					CallInTime = 2;
				}
				else
				{
					GSM_Huangup();
				}
			}
//			if(--CallInTime == 0)
//			{
//				PrintString3("ATA\r\n");
//				SysStatus = SYS_TALK;
//				GsmStatus = GSM_ST_TALK;
//				//CallOutTime = SysPara.TalkTime;
//				TalkTime = SysPara.TalkTime;
//				Volum = SysPara.TalkVolum;
//				GSM_SetVolum(Volum);
//				PA_PWR_L;
//			}
			break;
		case 5:
			{
				i = lookforstr(p,rspStrTable[5]);
				if(i != -1)
				{
					p += i+strlen(rspStrTable[5]);
					sscanf(p,"%bd,%bd,%bd",&i,&j,&stat);
					printf("CLCC: %bd\r\n",stat);
					
					if(stat == 0)//接通
					{
						SysStatus = SYS_TALK;
						GsmStatus = GSM_ST_TALK;
						//CallOutTime = SysPara.TalkTime;
						CallOutTime = 0;
						TalkTime = SysPara.TalkTime;
						Volum = SysPara.TalkVolum;
						GSM_SetVolum(Volum);
						PA_PWR_L;
					}
					else if(stat == 3)//呼叫通
					{
						if(GsmCallOk == false)
						{
							GsmCallOk = true;
							CallOutTime = SysPara.RingOutTs;
						}
						//PA_PWR_L;
						if(++PaOpenTime == 5)
						{
							printf("\r\n----PaOpen----\r\n");
							PA_PWR_L;
						}
						//printf("PaT: %bd\r\n",PaOpenTime);
					}
					else if(stat == 6)//挂断
					{
						if(GsmStatus == GSM_ST_WAITHUNGUP)
						{
							return;
						}
						GSM_Huangup();
						SysIdleCon();
					}
					break;
				}
			}
			break;
		case 8:
//			SysStatus = SYS_TALK;
//			GsmStatus = GSM_ST_TALK;
//			//CallOutTime = SysPara.TalkTime;
//			TalkTime = SysPara.TalkTime;
//			Volum = SysPara.TalkVolum;
//			GSM_SetVolum(Volum);
//			PA_PWR_L;
			break;
		case 6:
			{
				i = lookforstr(p,rspStrTable[6]);
				if(i != -1)
				{
					p += i+strlen(rspStrTable[6]);
					sscanf(p,"%bd",&stat);
					DtmfData = stat-48;
					printf("DTMF: %bd\r\n",stat);
					break;
				}
			}
			break;
		case 7:
			{
				i = lookforstr(p,rspStrTable[7]);
				if(i != -1)
				{
					p += i+strlen(rspStrTable[7]);
					sscanf(p,"%bd",&stat);
					DtmfData = stat-48;
					printf("DTMF: %bd\r\n",stat);
					break;
				}
			}
			break;
		case 9:
			{
				i = lookforstr(p,(uint8_t *)rspStrTable[9]);
				if(i != -1)
				{
					p += i+strlen((uint8_t *)rspStrTable[9]);
					sscanf(p,"%bd",&stat);
					DtmfData = stat-48;
					printf("DTMF: %bd\r\n",DtmfData);
					break;
				}
			}
			break;
	}
}

void GsmUARTCmd(unsigned char * buf,uint16_t len)
{
	AtCmdRspType CmdRes;
	buf[len] = 0;
	if((GsmStatus == GSM_ST_WAITAT) && (AtCmdIdx < GSM_AT_COUNT))
	{
		if(AtCmdBuf[AtCmdIdx].p_atCmdCallBack != NULL)
		{
			CmdRes = AtCmdBuf[AtCmdIdx].p_atCmdCallBack(buf);
			switch(CmdRes)
			{
				case AT_RSP_CONTINUE:
				case AT_RSP_FUN_OVER:
					AtCmdRetryTime = 0;
					if((AtCmdIdx+1) < GSM_AT_COUNT)
					{
						AtCmdIdx++;
						PrintString3(AtCmdBuf[AtCmdIdx].p_atCmdStr);
						GsmInitTime = 1000;
						printf("GSM_AtCmd:%bd - %bd\r\n",AtCmdIdx,AtCmdRetryTime);
					}
					else
					{
						GsmInitTime = 10000;
						GsmStatus = GSM_ST_ATOVER;
						if(SysStatus == SYS_TALK)
						{
							GsmInitTime = 1000;
							GsmStatus = GSM_ST_TALK;
						}
					}
					
					break;
				case AT_RSP_WAIT:
					
					break;
				case AT_RSP_ERROR:
					if(SysStatus == SYS_TALK)
					{
						GsmInitTime = 1000;
						GsmStatus = GSM_ST_TALK;
					}
					else
					{
						GSM_Repower();
					}
					break;
			}
		}
	}
	else
	{
		ModemReceiveHandle(buf);
	}
}

void GSM_send_dtmf(uint8_t k)
{
	uint8_t dtmf_cmd[]="AT+VTS=\"0\"\r\n";
	dtmf_cmd[8] = k+'0';
//	uint8_t dtmf_cmd[]="AT+QWDTMF=1,0,\"1,2,3,A,B\",500\r\n";
	//dtmf_cmd[8] = k+'0';
	PrintString3(dtmf_cmd);
}
