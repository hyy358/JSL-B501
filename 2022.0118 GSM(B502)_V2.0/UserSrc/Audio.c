#include "config.h"
#include "Driver.h"
#include "Audio.h"
#include "stdio.h"
#include "GPIO.h"
#include "UART.h"
#include "SysStatus.h"

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
	SPI_InitStructure.SPI_Module    = ENABLE;						//SPI启动    ENABLE, DISABLE
	SPI_InitStructure.SPI_SSIG      = DISABLE;					//片选位     ENABLE, DISABLE
	SPI_InitStructure.SPI_FirstBit  = SPI_MSB;					//移位方向   SPI_MSB, SPI_LSB
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;	  //主从选择   SPI_Mode_Master, SPI_Mode_Slave
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;    //时钟相位   SPI_CPOL_High,   SPI_CPOL_Low
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge;	  //数据边沿   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
	SPI_InitStructure.SPI_Interrupt = ENABLE;			  //中断允许   ENABLE,DISABLE
	SPI_InitStructure.SPI_Speed     = SPI_Speed_4;		  //SPI速度    SPI_Speed_4, SPI_Speed_16, SPI_Speed_64, SPI_Speed_128
	SPI_InitStructure.SPI_IoUse     = SPI_P54_P40_P41_P43; //IO口切换   SPI_P12_P13_P14_P15, SPI_P22_P23_P24_P25, SPI_P54_P40_P41_P43, SPI_P35_P34_P33_P32
	SPI_Init(&SPI_InitStructure);
	
	SPI_FLASH_CS_DISEN;
}

#define PWMC (*(unsigned int volatile xdata *)0xfff0)
#define PWMCKS (*(unsigned char volatile xdata *)0xfff2)
#define PWM0T1 (*(unsigned int volatile xdata *)0xff00)
#define PWM0T2 (*(unsigned int volatile xdata *)0xff02)
#define PWM0CR (*(unsigned char volatile xdata *)0xff04)
sfr PWMCR = 0xfe;
#define PWM_CYCLE 0x00FF
#define PWM_STOP() {PWMCR = 0;}
#define PWM_START() {PWMCR = 0x80;}
void PWM_config(void)
{
	EAXSFR();
	PWMCKS = 0x00;
	PWMC = PWM_CYCLE;
	PWM0T1 = 0;
	PWM0T2 = (PWM_CYCLE/2);
	PWM0CR = 0x90;
	EAXRAM();
	PWM_STOP();
}

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

//WavFileType WavFile[MP3_Last-1]=
//{
//{0,51170},
//{51170,77254},
//{128424,133702},
//{262126,72646},
//{334772,42112},
//{376884,51766},
//{428650,64582},
//{493232,67476},
//{560708,56364},
//{617072,210944},
//};
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
void WavReadTask(void)
{
	uint8_t midx;
//	uint16_t i,tmp;
	if(WavNeedRead)
	{
		WavNeedRead = false;
		midx = 1-Midx;
		if((WavFileLen == 0) || (WavReadIdx == WavFileLen))
		{
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
			WavBufLen[midx] = WavFileLen-WavReadIdx;
			WavFileLen = 0;
			if(((IdleTime > 0) && (IdleTime <= 3)) || ((SysStatus != SYS_TALK) && (SysStatus != SYS_CALL)))
			{
				PA_PWR_H;
			}
			
		}
		SPI_Flash_Read(WavBuf[midx],WavFileAddr+WavReadIdx,WavBufLen[midx]);
//		for(i=0;i<WavBufLen[midx];i++)
//		{
//			tmp = WavBuf[midx][i];
//			WavBuf[midx][i] = (tmp*SysPara.TalkVolum)/5;
//		}
	}
}

void PWMtask(void)
{
	//uint16_t tmp;
	if(PlyIdx < WavBufLen[Midx])
	{
		EAXSFR();
//		tmp = WavBuf[Midx][PlyIdx+1];
//		tmp <<= 8;
//		tmp |= WavBuf[Midx][PlyIdx];
//		PlyIdx += 2;
//		if(tmp < 0){PlyIdx += 2;return;}
		PWM0T2 = WavBuf[Midx][PlyIdx++];
		EAXRAM();
	}
	else
	{
		PlyIdx = 0;
		Midx = 1-Midx;
		WavNeedRead = true;
	}
}

void MP3_StartPlay(uint8_t idx)
{
	if(!SysPara.ToneEn)
	{
		return;
	}
//	PWM_STOP();
	Timer4_Stop();
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
	
	PWM_START();
	Timer4_Run();
	AudioPlaying = true;
}
void MP3_Stop(void)
{
	if(((IdleTime > 0) && (IdleTime <= 3)) || ((SysStatus != SYS_TALK) && (SysStatus != SYS_CALL)))
	{
		PA_PWR_H;
	}
	AudioPlaying = false;
//	PWM_STOP();
	Timer4_Stop();
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
				P62 = 0;
				return;
			}
			KeyBeepO = 1;
			BeepPlyIdx++;
		}
	}
}
#endif