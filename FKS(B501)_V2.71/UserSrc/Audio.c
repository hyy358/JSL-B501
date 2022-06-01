#include "config.h"
#include "Driver.h"
#include "Audio.h"
#include "stdio.h"
#include "GPIO.h"
#include "UART.h"
#include "SysStatus.h"
#include "WDT.h"

#ifdef MP3_MODULE
//bool Mp3Busy = true;
Mp3CmdType Mp3Cmd=
{
	0x7EFF,
	0x06,
	0x09,
	0x00,
	0x00,
	0x02,
	0xFF0F,
	0xEF,
};

void Mp3SendCmd(void)
{
	u8 *pD = (u8 *)&Mp3Cmd;
	u8 i;
	Mp3Cmd.Chk = 0;
	for(i=1;i<7;i++)
	{
		Mp3Cmd.Chk += pD[i];
	}
	Mp3Cmd.Chk = ~Mp3Cmd.Chk;
	Mp3Cmd.Chk++;
	printf("Mp3UartTx: ");
	for(i=0;i<sizeof(Mp3Cmd);i++)
	{
		printf("%b02X ",pD[i]);
	}
	printf("\r\n");
	Uart4SendBuf(pD,sizeof(Mp3Cmd));
}

void MP3_SetMode(void)
{
	Mp3Cmd.Len = 6;
	Mp3Cmd.Cmd = MP3_CMD_SETMODE;
	Mp3Cmd.Para1 = 0;
	Mp3Cmd.Para2 = 2;
	Mp3SendCmd();
}
void MP3_SetVolum(void)
{
	Mp3Cmd.Len = 6;
	Mp3Cmd.Cmd = MP3_CMD_SETVOLUM;
	Mp3Cmd.Para1 = 0;
	Mp3Cmd.Para2 = 30;
	Mp3SendCmd();
}

void MP3_StartPlay(u16 idx)
{
//	if(Mp3Busy)
//	{
//		return;
//	}
	Mp3Cmd.Len = 6;
	Mp3Cmd.Cmd = MP3_CMD_PLAY;
	Mp3Cmd.Para1 = (idx>>8)&0xFF;
	Mp3Cmd.Para2 = idx&0xFF;
	PA_PWR_L;
	Mp3SendCmd();
//	Mp3Busy = true;
}

void Mp3UartRx(u8 * buf)
{
	u8 i;
	u16 Chk = 0;
	Mp3CmdType *pTmp = (Mp3CmdType*)buf;
	printf("Mp3UartRx:");
	for(i=0;i<pTmp->Len+4;i++)
	{
		printf("%b02X ",buf[i]);
	}
	printf("\r\n");
	for(i=1;i<pTmp->Len+1;i++)
	{
		Chk += buf[i];
	}
	Chk = ~Chk;
	Chk++;
	if((pTmp->St == 0x7EFF) && (pTmp->End == 0xEF) && (Chk == pTmp->Chk))
	{
		switch(pTmp->Cmd)
		{
			case MP3_CMD_PWRON:
				if(pTmp->Para2 != 2)
				{
					MP3_SetMode();
				}
				else
				{
					//Mp3Busy = false;
					MP3_SetVolum();
				}
				break;
			case MP3_CMD_RE_STOP:
				if(((IdleTime > 0) && (IdleTime <= 3)) || ((SysStatus != SYS_TALK) && (SysStatus != SYS_CALL)))
				{
					PA_PWR_H;
				}
				//Mp3Busy = false;
				break;
			case MP3_CMD_RE_ERR:
				PA_PWR_H;
				//Mp3Busy = false;
				break;
		}
	}
}
#elif defined PWM_AUDIO

#include "spi.h"

void SPI_config(void)
{
	SPI_InitTypeDef		SPI_InitStructure;
	SPI_InitStructure.SPI_Module    = ENABLE;						//SPIÆô¶¯    ENABLE, DISABLE
	SPI_InitStructure.SPI_SSIG      = DISABLE;					//Æ¬Ñ¡Î»     ENABLE, DISABLE
	SPI_InitStructure.SPI_FirstBit  = SPI_MSB;					//ÒÆÎ»·½Ïò   SPI_MSB, SPI_LSB
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;	  //Ö÷´ÓÑ¡Ôñ   SPI_Mode_Master, SPI_Mode_Slave
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;    //Ê±ÖÓÏàÎ»   SPI_CPOL_High,   SPI_CPOL_Low
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge;	  //Êý¾Ý±ßÑØ   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
	SPI_InitStructure.SPI_Interrupt = ENABLE;			  //ÖÐ¶ÏÔÊÐí   ENABLE,DISABLE
	SPI_InitStructure.SPI_Speed     = SPI_Speed_4;		  //SPIËÙ¶È    SPI_Speed_4, SPI_Speed_16, SPI_Speed_64, SPI_Speed_128
	SPI_InitStructure.SPI_IoUse     = SPI_P54_P40_P41_P43; //IO¿ÚÇÐ»»   SPI_P12_P13_P14_P15, SPI_P22_P23_P24_P25, SPI_P54_P40_P41_P43, SPI_P35_P34_P33_P32
	SPI_Init(&SPI_InitStructure);
	
	SPI_FLASH_CS_DISEN;
}

#define PWMC (*(unsigned int volatile xdata *)0xfff0)
#define PWMCKS (*(unsigned char volatile xdata *)0xfff2)
sfr PWMCR = 0xfe;
#define PWM_CYCLE 0x00FF
#define PWM_STOP() {PWMCR = 0;}
#define PWM_START() {PWMCR = 0x80;}

#define PWM0T1 (*(unsigned int volatile xdata *)0xff00)
#define PWM0T2 (*(unsigned int volatile xdata *)0xff02)
#define PWM0CR (*(unsigned char volatile xdata *)0xff04)
//#define PWM0_STOP() {EAXSFR();PWM0CR = 0;EAXRAM();}
//#define PWM0_START() {EAXSFR();PWM0CR = 0x90;EAXRAM();}

#define PWM1T1 (*(unsigned int volatile xdata *)0xff10)
#define PWM1T2 (*(unsigned int volatile xdata *)0xff12)
#define PWM1CR (*(unsigned char volatile xdata *)0xff14)
//#define PWM1_STOP() {EAXSFR();PWM1CR = 0;EAXRAM();}
//#define PWM1_START() {EAXSFR();PWM1CR = 0x90;EAXRAM();}
#define PWM2T1 (*(unsigned int volatile xdata *)0xff20)
#define PWM2T2 (*(unsigned int volatile xdata *)0xff22)
#define PWM2CR (*(unsigned char volatile xdata *)0xff24)

void PWM_config(void)
{
	EAXSFR();
	PWMCKS = 0x00;
	PWMC = PWM_CYCLE;
	
//	PWM0T1 = 0;
//	PWM0T2 = (PWM_CYCLE/2);
//	PWM0CR = 0x90;
	
	PWM1T1 = 0;
	PWM1T2 = (PWM_CYCLE/2);
	PWM1CR = 0x90;
	
//	PWM2T1 = 0;
//	PWM2T2 = (PWM_CYCLE/2);
//	PWM2CR = 0x90;
	
	EAXRAM();
	
//	PWM0_START();
//	PWM1_START();
	
//	PWM_STOP();
}


//void PWM_config(void)
//{
//	EAXSFR();
//	PWMCKS = 0x00;
//	PWMC = PWM_CYCLE;
//	PWM1T1 = 0;
//	PWM1T2 = (PWM_CYCLE/2);
//	PWM1CR = 0x90;
//	EAXRAM();
//	
//	PWM_STOP();
//}


#define WAV_BUFLEN 1024
uint8_t WavBuf[2][WAV_BUFLEN];
static uint16_t PlyIdx = 0;
static uint8_t Midx = 0;
uint16_t WavBufLen[2] = {0,0};
uint32_t WavFileAddr = 0;
uint32_t WavFileLen = 0;
uint32_t WavReadIdx = 0;
bool WavNeedRead = false;
bool AudioPlaying = false;

void PWMtask(void)
{
	if(!AudioPlaying)
	{
		return;
	}
	if(PlyIdx < WavBufLen[Midx])
	{
		EAXSFR();
		PWM1T2 = WavBuf[Midx][PlyIdx];
	//	PWM2T2 = WavBuf[Midx][PlyIdx];
		PlyIdx++;
		EAXRAM();
	}
	else
	{
		if((WavFileLen == 0) || (WavReadIdx == WavFileLen))
		{
			MP3_Stop();
			return;
		}
		PlyIdx = 0;
		Midx = 1-Midx;
		WavNeedRead = true;
	}
}

//void PWMtask(void)
//{
//	if(PlyIdx < WavBufLen[Midx])
//	{
//		EAXSFR();
//		PWM1T2 = WavBuf[Midx][PlyIdx++];
//		EAXRAM();
//	}
//	else
//	{
//		PlyIdx = 0;
//		Midx = 1-Midx;
//		WavNeedRead = true;
//	}
//}

WavFileType WavFile[MP3_Last-1]=
{
{0,68012},
{68012,53036},
{121048,46124},
{167172,57644},
{224816,58796},
{283612,50732},
{334344,58796},
{393140,55340},
{448480,42668},
{491148,192386},
{683534,34604},
{718138,36908},
{755046,57644},
{812690,24236},
{836926,62252},
{899178,73772},
{972950,12842},
};
//u16 last = 0;
//u16 tmp = 0;

void WavReadTask(void)
{
	uint8_t midx;
	if(WavNeedRead)
	{
		WDT_Clear();
		WavNeedRead = false;
//		tmp = WavBuf[midx][WavBufLen[midx]-1];
		midx = 1-Midx;
		if((WavFileLen == 0) || (WavReadIdx == WavFileLen))
		{
//			if(last == 0)
//			{
//				WavBuf[midx][0] = tmp;
//				WavBufLen[midx] = tmp*4;
//				for(last=1;last<WavBufLen[midx];last++)
//				{
//					WavBuf[midx][last] = WavBuf[midx][0]-(last/4);
//				}
//				last = 1;
//				return;
//			}
			
			MP3_Stop();
			return;
		}
		if((WavFileLen-WavReadIdx) >  WAV_BUFLEN)
		{
			WavBufLen[midx] = WAV_BUFLEN;
			WavReadIdx += WAV_BUFLEN;
		}
		else
		{
			MP3_Stop();
			return;
//			WavBufLen[midx] = WavFileLen-WavReadIdx;
//			WavFileLen = 0;
//			if(((IdleTime > 0) && (IdleTime <= 3)) || ((SysStatus != SYS_TALK) && (SysStatus != SYS_CALL)))
//			{
//				PA_PWR_H;
//			}
			
		}
		SPI_Flash_Read(WavBuf[midx],WavFileAddr+WavReadIdx,WavBufLen[midx]);
	}
}

void MP3_StartPlay(uint8_t idx)
{
	if((!SysPara.ToneEn))// || (AudioPlaying))
	{
		return;
	}
	PWM_STOP();
//	Timer4_Stop();
	WDT_Clear();
	PA_PWR_L;
	delay(500);
	WavFileAddr = WavFile[idx-1].FileAddr;
	WavFileLen = WavFile[idx-1].FileLen;
	
	PlyIdx = 0;
	WavReadIdx = 0;
	WavBufLen[0] = 0;
	WavBufLen[1] = 0;
	
	Midx = 1;
	WavNeedRead = true;
	WavReadTask();
	
	Midx = 0;
	WavNeedRead = true;
	WavReadTask();
	
//	last = 0;
P61_PWM;	
	PWM_START();
//	Timer4_Run();
	AudioPlaying = true;
//	RING_E_H;
}
void MP3_Stop(void)
{
	if(((IdleTime > 0) && (IdleTime <= 3)) || ((SysStatus != SYS_TALK) && (SysStatus != SYS_CALL)))
	{
		PA_PWR_H;
	}
//	RING_E_H;
	AudioPlaying = false;
P61_IN;
	PWM_STOP();
//P61=1;
//	Timer4_Stop();
}
#endif

#ifdef PWM_BEEP

#define PWMC (*(unsigned int volatile xdata *)0xfff0)
#define PWMCKS (*(unsigned char volatile xdata *)0xfff2)
sfr PWMCR = 0xfe;

#define PWM_CYCLE 0x03FF
#define PWM_STOP() {PWMCR = 0;}
#define PWM_START() {PWMCR = 0x80;}

#define PWM0T1 (*(unsigned int volatile xdata *)0xff00)
#define PWM0T2 (*(unsigned int volatile xdata *)0xff02)
#define PWM0CR (*(unsigned char volatile xdata *)0xff04)
#define PWM0_STOP() {EAXSFR();PWM0CR = 0;EAXRAM();}
#define PWM0_START() {EAXSFR();PWM0CR = 0x90;EAXRAM();}


void PWM_config(void)
{
	EAXSFR();
	PWMCKS = 0x08;
	PWMC = PWM_CYCLE;
	PWM0T1 = 0;
	PWM0T2 = (PWM_CYCLE/2);
	PWM0CR = 0x90;
	EAXRAM();
	//PWM0_STOP();
	//PWM_STOP();
}

code WavFileType WavCode[] = {
{3,{3,2,1}},//OK
{5,{1,0,1,0,1}},//Err
{7,{1,1,1,0,1,0,1}},//Unlock
{3,{1,0,1}},//Call
{1,{1,2}},//Key
};

u8 PlyIdx = 0xFF;
u8 PlyN = 0;
u8 PwmTimer = 0;

void MP3_StartPlay(u8 idx)
{
	//PlyN = idx;
	switch(idx)
	{
		case MP3_InputRoom:
		case MP3_InputPsw:
		case MP3_Key:
			PlyN = 4;
			break;
		case MP3_SetOK:
			PlyN = 0;
			break;
		case MP3_Unlock:
			PlyN = 2;
			break;
		case MP3_RoomErr:
		case MP3_PwdErr:
		case MP3_CardErr:
		case MP3_Noresponse:
			PlyN = 1;
			break;
		case MP3_Huangup:
		case MP3_Ring:
			PlyN = 3;
			break;
	}
	
	PlyIdx = 0;
	PwmTimer = 99;
	PWMtask();
	PWM_START();
}

void PWMtask(void)
{
	u16 tmp;
	if(PlyIdx != 0xFF)
	{
		if(++PwmTimer == 100)
		{
			PwmTimer = 0;
			if(PlyIdx >= WavCode[PlyN].Len)
			{
				PlyIdx = 0xFF;
				PWM_STOP();
				return;
			}
			EAXSFR();
			tmp = WavCode[PlyN].Buf[PlyIdx];
			if(tmp != 0)
			{
				tmp = PWM_CYCLE+(200*tmp);
			}
			PWMC = tmp;
			PWM0T1 = 0;
			PWM0T2 = (tmp/2);
			EAXRAM();
			PlyIdx++;
		}
	}
}
#endif

#ifdef KEY_BEEP

code BeepFileType WavCode[] = {
{3,{3,2,1}},//OK
{5,{1,0,1,0,1}},//Err
{7,{1,1,1,0,1,0,1}},//Unlock
{3,{1,0,1}},//Call
{1,{1,2}},//Key
};

u8 BeepPlyIdx = 0xFF;
u8 PlyN = 0;
u8 PwmTimer = 0;
bit KeyBeepO = 0;
void Beep_StartPlay(u8 idx)
{
	switch(idx)
	{
		case MP3_InputRoom:
		case MP3_InputPsw:
		case MP3_Key:
			PlyN = 4;
			break;
		case MP3_SetOK:
			PlyN = 0;
			break;
		case MP3_Unlock:
			PlyN = 2;
			break;
		case MP3_RoomErr:
		case MP3_PwdErr:
		case MP3_CardErr:
		case MP3_Noresponse:
			PlyN = 1;
			break;
		case MP3_Huangup:
		case MP3_Ring:
			PlyN = 3;
			break;
	}
	
	BeepPlyIdx = 0;
	PwmTimer = 99;
	KeyBeepTask();
	KeyBeepO = 1;
	RING_E_L;
}

void KeyBeepTask(void)
{
	if(BeepPlyIdx != 0xFF)
	{
		if(++PwmTimer == 100)
		{
			PwmTimer = 0;
			if(BeepPlyIdx >= WavCode[PlyN].Len)
			{
				BeepPlyIdx = 0xFF;
				KeyBeepO = 0;
				P60 = 0;
				return;
			}
			KeyBeepO = 1;
			BeepPlyIdx++;
		}
	}
}
#endif

#ifdef PWM_RING
u8 RingO = 0;
u8 RingTimer = 0;
u8 RingTs = 0;
u16 RingTime = 0;
void StartRing(u16 t)
{
	RingTime = t;
	RingTs = 0;
}
void RingTask(void)
{
	if(RingTime)
	{
		if(++RingTimer == 75)
		{
			RingTimer = 0;
			RingO = (RingO == 7)?5:7;
			RingTs++;
			if(RingTs >= 10)
			{
				RingO = 0;
			}
			if(RingTs >= 25)
			{
				RingTs = 0;
				P63 = 0;
				RingTime--;
			}
		}
	}
}
#endif
