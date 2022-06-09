#include "config.h"
#include "Driver.h"
#include "stdio.h"
#include "GPIO.h"
#include "UART.h"
#include "timer.h"
#include "Exti.h"
#include "SysString.h"
#include "SysStatus.h"
#include "Audio.h"

uint8_t DtmfData = 0;

void delay(u16 t)
{
	u16 i,j;
	t*=7;
	for(i=0;i<t;i++)
		for(j=0;j<110;j++);
} 

char putchar(char ch)
{
	TX1_write2buff(ch);	
	return ch;
}
uint8_t strlen(uint8_t *pch)
{
	uint8_t len = 0;
	while(*pch++)len++;
	return len;
}
uint8_t HBstrlen(uint8_t *pch)
{
	uint8_t len = 0;
	while(*pch)
	{
		len++;
		if(*pch == 0xD7)
		{
			len--;
		}
		pch++;
	}
	return len;
}

int lookforstr(uint8_t *srcBuf,uint8_t *detBuf)
{
	int i,j;
	i = 0;
	j = 0;
	while(srcBuf[i])
	{
		if(srcBuf[i] == detBuf[0])
		{
			for(j=0;detBuf[j];j++)
			{
				if(srcBuf[i+j] != detBuf[j])
				{
					break;
				}
			}
			if(detBuf[j] == 0)
			{
				return i;
			}
		}
		i++;
	}
	return -1;
}

void ClearBuf(uint8_t *buf,uint16_t len)
{
	while(len--)
	{
		*buf++ = 0;
	}
}
void HS_memcpy(uint8_t * dst,uint8_t * src,uint16_t len)
{
	while(len--)
	{
		*dst++ = *src++;
	}
}
uint16_t HS_strcpy(uint8_t * dst,uint8_t * src)
{
	uint16_t i = 0;
	while(*src)
	{
		*dst++ = *src++;
		i++;
	}
	*dst = 0;
	return i;
}
bool MemIsEmpty(uint8_t *buf,uint16_t len)
{
	while(len--)
	{
		if(*buf++ != 0)
		{
			return false;
		}
	}
	return true;
}

void GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;		//结构定义
	GPIO_InitStructure.Pin  = GPIO_Pin_All;		//指定要初始化的IO,
	
//============GPIO_PullUp===========================================================
	
	GPIO_InitStructure.Mode = GPIO_PullUp;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//初始化

	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);
	
	//GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  =  GPIO_Pin_0;
	GPIO_Inilize(GPIO_P4,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Inilize(GPIO_P6,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_3 | GPIO_Pin_6;
	GPIO_Inilize(GPIO_P7,&GPIO_InitStructure);
	
//============GPIO_OUT_PP===========================================================
	GPIO_InitStructure.Mode = GPIO_OUT_PP;
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4;
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  =  GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Inilize(GPIO_P4,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2  | GPIO_Pin_4  | GPIO_Pin_5;
	GPIO_Inilize(GPIO_P7,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_2;
	GPIO_Inilize(GPIO_P6,&GPIO_InitStructure);
	
	GSM_PWREN_L;
	GSM_PWRKEY_L;
	
	IIC_CLK_H;
	IIC_DAT_H;
	IIC_WP_H;
	
	RELAY_1_OFF;
	RELAY_2_OFF;
	
//	RFID_SHD_L;
}

void UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//使用波特率, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = Polity_1;			//指定中断优先级(低到高) Polity_0,Polity_1,Polity_2,Polity_3
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
	UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口1 UART1,UART2,UART3,UART4
	printf("UART1_config OK!\r\n");	//UART1发送一个字符串

#ifdef UART3	
	COMx_InitStructure.UART_P_SW      = UART3_SW_P50_P51;	//切换端口
	UART_Configuration(UART3, &COMx_InitStructure);		//初始化串口 UART1,UART2,UART3,UART4
	TX3_write2buff('X');
	printf("UART3_config OK!\r\n");
#endif
	
#ifdef UART4	
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer4;	
	COMx_InitStructure.UART_BaudRate  = 9600ul;
	COMx_InitStructure.UART_P_SW      = UART4_SW_P02_P03;	//切换端口
	UART_Configuration(UART4, &COMx_InitStructure);		//初始化串口 UART1,UART2,UART3,UART4
	TX4_write2buff('X');
	printf("UART4_config OK!\r\n");
#endif
}

void Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;						//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = Polity_0;			//指定中断优先级(低到高) Polity_0,Polity_1,Polity_2,Polity_3
	TIM_InitStructure.TIM_Interrupt = ENABLE;					//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 1000);		//初值,
	TIM_InitStructure.TIM_Run       = ENABLE;					//是否初始化后启动定时器, ENABLE或DISABLE

	Timer_Inilize(Timer0,&TIM_InitStructure);					//初始化Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4
	
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 100000);		//初值,
	Timer_Inilize(Timer1,&TIM_InitStructure);					//初始化Timer1	  Timer0,Timer1,Timer2,Timer3,Timer4
#ifdef PWM_AUDIO	
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 32000);		//初值,
	Timer_Inilize(Timer4,&TIM_InitStructure);
#endif
}

void Exti_config(void)
{
	EXTI_InitTypeDef	Exti_InitStructure;							//结构定义

	Exti_InitStructure.EXTI_Interrupt = ENABLE;				//中断使能,   ENABLE或DISABLE
	Exti_InitStructure.EXTI_Polity    = Polity_0;			//指定中断优先级(低到高) Polity_0,Polity_1,Polity_2,Polity_3
	Exti_InitStructure.EXTI_Mode      = EXT_MODE_RiseFall;//中断模式,   EXT_MODE_RiseFall,EXT_MODE_Fall
	Ext_Inilize(EXT_INT0,&Exti_InitStructure);				//初始化
#ifdef EXT_INT1
	Exti_InitStructure.EXTI_Mode      = EXT_MODE_Fall;//中断模式,   EXT_MODE_RiseFall,EXT_MODE_Fall
	Ext_Inilize(EXT_INT1,&Exti_InitStructure);				//初始化
#endif
}

uint8_t KeyScan(void)
{
	static uint8_t KeyStep = 0;
	static uint8_t KeyTime = 0;
	uint8_t key;
	switch(KeyStep)
	{
		case 0:
			KEY_P = 0xF0;
			if((KEY_P&0xF0) != 0xF0)
			{
				KeyTime = 20;
				KeyStep = 1;
			}
			break;
		case 1:
			if((KEY_P&0xF0) == 0xF0)
			{
				KeyStep = 0;
			}
			else if(--KeyTime == 0)
			{
				KeyStep = 2;
			}
			break;
		case 2:
			key = KEY_P | 0x0F;
			KEY_P = key;
			KeyStep = 3;
			break;
		case 3:
			key = KEY_P;
			KEY_P = 0xF0;
			KeyStep = 4;
			switch(key)
			{
				case 0xEE:
					return 4;
				case 0xED:
					return 8;
				case 0xEB:
					return 11;
				case 0xE7:
					return 15;
				case 0xDE:
					return 3;
				case 0xDD:
					return 7;
				case 0xDB:
					return 0;
				case 0xD7:
					return 14;
				case 0xBE:
					return 2;
				case 0xBD:
					return 6;
				case 0xBB:
					return 10;
				case 0xB7:
					return 13;
				case 0x7E:
					return 1;
				case 0x7D:
					return 5;
				case 0x7B:
					return 9;
				case 0x77:
					return 12;
			}
			break;
		case 4:
			if((KEY_P&0xF0) == 0xF0)
			{
				KeyStep = 5;
				KeyTime = 20;
			}
			break;
		case 5:
			if((KEY_P&0xF0) != 0xF0)
			{
				KeyStep = 4;
			}
			else if(--KeyTime == 0)
			{
				KeyStep = 0;
			}
			break;
					
	}
	return 0xFF;
}

uint8_t SkeyStatus = 0x1F;
uint8_t SkeyStatusOld = 0x1F;
uint16_t ResetKeyTime = 0;
void SkeyScan(void)
{
	static uint8_t SkeyStep = 0;
	static uint8_t SkeyTime = 0;
	
	uint8_t key = 0;
	switch(SkeyStep)
	{
		case 0:
			key = K_MAG;key<<=1;
			key |= K_DET;key<<=1;
			key |= K_RESET;key<<=1;
			key |= K_REX2;key<<=1;
			key |= K_REX1;
		
			if(key != SkeyStatus)
			{
				SkeyTime = 30;
				SkeyStep = 1;
			}
			break;
		case 1:
			key = K_MAG;key<<=1;
			key |= K_DET;key<<=1;
			key |= K_RESET;key<<=1;
			key |= K_REX2;key<<=1;
			key |= K_REX1;
		
			if(key == SkeyStatus)
			{
				SkeyStep = 0;
			}
			else if(--SkeyTime == 0)
			{
				SkeyStep = 0;
				SkeyStatus = key;
			}
			break;
					
	}
	if(SkeyStatusOld != SkeyStatus)
	{
		
		if(((SkeyStatus & 0x01) != (SkeyStatusOld&0x01)) && ((SkeyStatus & 0x01) == 0))//K_REX1
		{
			Unlock(0,SysPara.RealyTime,false);
			IdleTime = 5;
		}
		if(((SkeyStatus & 0x02) != (SkeyStatusOld&0x02)) && ((SkeyStatus & 0x02) == 0))//K_REX2
		{
			Unlock(1,SysPara.RealyTime,false);
			IdleTime = 5;
		}
		if((SkeyStatus & 0x04) != (SkeyStatusOld&0x04))//K_RESET
		{
			ResetKeyTime = 10000;
			printf("ResetKeyTime\r\n");
		}
		if((SkeyStatus & 0x08) != (SkeyStatusOld&0x08))//K_DET
		{
			
		}
		if((SkeyStatus & 0x10) != (SkeyStatusOld&0x10))//K_MAG
		{
			
		}
		SkeyStatusOld = SkeyStatus;
	}
	if((SkeyStatusOld&0x04) == 0)
	{
		if(ResetKeyTime)
		{
			if(--ResetKeyTime == 0)
			{
				InitSysPara(true);
				ShowReset();
				IdleTime = 5;
			}
		}
	}
}

uint8_t Rly1Time[2] = {0,0};
void Unlock(uint8_t idx,uint8_t time,bool sound)
{
	Rly1Time[idx] = time;
	if(idx == 0)
	{
		RELAY_1_ON;
	}
	else if(idx == 1)
	{
		RELAY_2_ON;
	}
	ShowUnlock(sound);
}
void RelayTask(void)//1sec
{
	if(Rly1Time[0] != 0)
	{
		if(--Rly1Time[0] == 0)
		{
			RELAY_1_OFF;
		}
	}
	if(Rly1Time[1] != 0)
	{
		if(--Rly1Time[1] == 0)
		{
			RELAY_2_OFF;
		}
	}
}
