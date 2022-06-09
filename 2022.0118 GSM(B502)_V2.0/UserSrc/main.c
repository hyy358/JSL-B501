#include "Driver.h"
#include "stdio.h"
#include "UART.h"
#include "timer.h"
#include "Lcd12864.h"
#include "GSM.h"
#include "NameList.h"
#include "SysStatus.h"
#include "Eeprom.h"
#include "RfidCard.h"
#include "SysString.h"
#include "Audio.h"

uint16_t MainTime = 0;
uint8_t UsrKey = 0;
//extern GsmStatusType GsmStatus;
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
	delay(1000);
	Init_12864();
	EA = 1;
	delay(100);
	InitSysPara(false);
	InitUserPara();
	delay(100);
	GSM_StartInit();
	CardReadInit();
	SysIdleCon();
//	MP3_StartPlay(MP3_Key);
	//WdtConfig();
	//printf("EEPROM_ADDR_END,%d\r\n",EEPROM_ADDR_END);
	//RevNamelist("C:2;R:1001;N:HuangSheng;T1:18859119836;T2:123;T3:456;R:1002;N:wang;T1:17750651602;T2:;T3:;");
	while(1)
	{
		WG_Read26();
		WG_Send26();
		if(COM1.B_RX_OK)
		{
		//	printf("%s\r\n",RX1_Buffer);
			RevNamelist(RX1_Buffer);
			Uart1Clear();
		}
		if(COM3.B_RX_OK)
		{
			printf("%s",RX3_Buffer);
			GsmUARTCmd(RX3_Buffer,COM3.RX_Cnt);
			Uart3Clear();
		}
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
		if(DtmfData)
		{
			printf("DtmfData: 0x%b02X\r\n",DtmfData);
			if(DtmfData == SysPara.UnlockKey)
			{
				Unlock(0,SysPara.RealyTime,true);
			}
			if(DtmfData == SysPara.UnlockKey2)
			{
				Unlock(1,SysPara.RealyTime,true);
			}
			DtmfData = 0;
		}
		if(Timer0_Irq)
		{
			Timer0_Irq = false;
			
			UartTask();
			UsrKey = KeyScan();
			if(UsrKey!=0xFF)
			{
				//BeepOne();
			#ifdef KEY_BEEP
				Beep_StartPlay(MP3_Key);
			#else
				MP3_StartPlay(MP3_Key);
			#endif
				printf("UsrKey %bd\r\n",UsrKey);
				SysKey(UsrKey);
			}
			SkeyScan();
			
			CardMsTask();
			GSM_InitTask();
			FlashTask();
			if(++MainTime > 1000)
			{
				
				MainTime = 0;
				SYS_LED = ~SYS_LED;
				RelayTask();
				if(IdleTime && (CallOutTime == 0) && (TalkTime == 0))
				{
					if(--IdleTime == 0)
					{
						SysIdleCon();
					}
				}
				if(SysStatus == SYS_TALK)
				{
					ShowTalkTime();
					//printf("TalkTime: %d\r\n",TalkTime);
					if(TalkTime && (--TalkTime == 0))
					{
						GSM_Huangup();
						SysIdleCon();
					}
				}
			#ifdef LANGUAGE_HB
				MainShow++;
				MainShow %= 10;	
				if((IdleTime == 0) && (SysStatus == SYS_IDLE) && (NameListIdx == 0xFFFF))
				{
					ShowMain(MainShow);
				}
			#endif
				
//				if(PaOpenTime)
//				{
//					if(--PaOpenTime == 0)
//					{
//						PA_PWR_L;
//					}
//				}
			}
			
			if((EditTime > 0) && (SysStatus == SYS_EDIT_NAME))
			{
				EditTime--;
				if(EditTime == 0)
				{
					NameEditKey(KEY_TIMEOUT);
				}
			}
			
		}
		
		if((GsmCsqFlag) && (SysStatus == SYS_IDLE))
		{	
//			if(SysStatus == SYS_IDLE)
//			{
//				ShowGsmCsq();
//			}
			GsmCsqFlag = false;
		}
		
	
		
		if(CardReadOK)
		{
			
			//CheckRfidCard();
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
	}
}
