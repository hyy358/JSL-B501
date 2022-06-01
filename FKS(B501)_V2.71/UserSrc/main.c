#include "Driver.h"
#include "stdio.h"
#include "UART.h"
#include "timer.h"
#include "Lcd12864.h"
#include "NameList.h"
#include "SysStatus.h"
#include "Eeprom.h"
#include "RfidCard.h"
#include "SysString.h"
#include "Audio.h"
#include "WDT.h"

int MainTime = 0;
u8 MainShow = 0;
volatile int Key;
//SysStatusType SysStatusOld;

void main(void)
{
	delay(100);
	EA = 0;
	while(K_RESET == 0);
	GPIO_config();
	Timer_config();
	UART_config();
	Exti_config();
#ifdef PWM_AUDIO
	SPI_config();
	PWM_config();
#endif
#ifdef PWM_BEEP
	PWM_config();
#endif
	delay(1000);
	Init_12864();
	EA = 1;
	delay(100);
	InitSysPara(false);
	InitUserPara();
	delay(100);
	WDT_config();
	SysIdleCon();
	CardReadInit();
#ifdef PWM_BEEP
//#if 1	//(defined(PWM_BEEP) || defined(KEY_BEEP))
	MP3_StartPlay(MP3_SetOK);
#endif	
#ifdef KEY_BEEP
	Beep_StartPlay(MP3_SetOK);
#else
	MP3_StartPlay(MP3_SetOK);
#endif
	while(1)
	{
		if(COM1.B_RX_OK)
		{
		//	printf("%s\r\n",RX1_Buffer);
			RevNamelist(RX1_Buffer);
			Uart1Clear();
		}
#ifdef UART3		
		if(COM3.B_RX_OK)
		{
			printf("%s",RX3_Buffer);
			
			Uart3Clear();
		}
#endif
#ifdef MP3_MODULE
		if(COM4.B_RX_OK)
		{
			Mp3UartRx(RX4_Buffer);
			Uart4Clear();
		}
#endif	
#ifdef PWM_AUDIO		
		WavReadTask();
#endif
//		if(DtmfData)
//		{
//			printf("DtmfData: 0x%b02X\r\n",DtmfData);
//			if(DtmfData == SysPara.UnlockKey)
//			{
//				Unlock();
//			}
//			DtmfData = 0;
//		}
		if(Timer0_Irq)
		{
			Timer0_Irq = false;
			
			UartTask();
			FlashTask();
			if(++MainTime > 1000)
			{
				MainTime = 0;
				WDT_Clear();
				SYS_LED = ~SYS_LED;
				RelayTask();
				if(IdleTime)
				{
					if(--IdleTime == 0)
					{
						SysIdleCon();
					}
				}
				if(SysStatus == SYS_TALK)
				{
					if(--CallOutTime == 0)
					{
						SysIdleCon();
					}
					else if(UnlockTime == 0)
					{
						ShowTalkTime();
					}
				}
				if(SysStatus == SYS_CALL)
				{
					if(--CallOutTime == 0)
					{
//						SysStatus = SYS_TALK;
//						CallOutTime = SysPara.TalkTime;
//						ShowTalkTime();
						SysIdleCon();
					}
				}
			#ifdef LANGUAGE_HB
				MainShow++;
				MainShow %= 10;	
				if((SysStatus == SYS_IDLE) && (IdleTime == 0) && (NameListIdx == 0xFFFF) && ((MainShow == 5) || (MainShow == 0)))
				{
					ShowMain(MainShow==0);
				}
			#endif
			}

			if((EditTime > 0) && (SysStatus = SYS_EDIT_NAME))
			{
				EditTime--;
				if(EditTime == 0)
				{
					NameEditKey(KEY_TIMEOUT);
				}
			}
			Key = KeyScan();
			if(Key != 0xFF)
			{
				//MP3_StartPlay(MP3_InputRoom);
			#ifdef KEY_BEEP
				Beep_StartPlay(MP3_Key);
			#else
				MP3_StartPlay(MP3_Key);
			#endif
				SysKey(Key);
			}
			SkeyScan();
			CardMsTask();
		}
		
		if(CardReadOK)
		{
			CheckRfidCard();
			if(CardGetOk)
			{
				#ifdef KEY_BEEP
					Beep_StartPlay(MP3_Key);
				#else
					MP3_StartPlay(MP3_Key);
				#endif
				SysCard(CardCode);
			}
			CardReadInit();
		}
		
//		if(SysStatusOld != SysStatus)
//		{
//			printf("%bd ->> %bd\r\n",SysStatusOld,SysStatus);
//			SysStatusOld = SysStatus;
//		}
	}
}
