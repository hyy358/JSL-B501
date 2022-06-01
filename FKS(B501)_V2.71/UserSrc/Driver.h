#ifndef __DRIVER_H__
#define __DRIVER_H__
#include "config.h"

#define SYS_LED P41

#define LCD12864_E_H	{P52 = 1;}
#define LCD12864_E_L	{P52 = 0;}
#define LCD12864_WR_H	{P53 = 1;}
#define LCD12864_WR_L	{P53 = 0;}
#define LCD12864_RS_H	{P44 = 1;}
#define LCD12864_RS_L	{P44 = 0;}
#define LCD12864_DATA	P1

#define KEY_P P2

#define PA_PWR_H	{P64 = 1;}
#define PA_PWR_L	{P64 = 0;}

#define RING_E_H	{P63 = 1;}
#define RING_E_L	{P63 = 0;}

#define P61_PWM	{P6M1 &= ~GPIO_Pin_1,	P6M0 &= ~GPIO_Pin_1;}
#define P61_IN	{P6M1 |=  GPIO_Pin_1,	P6M0 &= ~GPIO_Pin_1;}

#define IIC_WP_H	{P67 = 1;}
#define IIC_WP_L	{P67 = 0;}
#define IIC_CLK_H	{P66 = 1;}
#define IIC_CLK_L	{P66 = 0;}
#define IIC_DAT_H	{P65 = 1;}
#define IIC_DAT_L	{P65 = 0;}
#define IIC_DAT		P65

#define RELAY_1_ON	{P43 = 1;}
#define RELAY_1_OFF	{P43 = 0;}

#define RFID_DAT	P32

#define CAMERA_ON	{P42 = 1;}
#define CAMERA_OFF	{P42 = 0;}

#define SPI_FLASH_CS_EN {P74 = 0;}
#define SPI_FLASH_CS_DISEN {P74 = 1;}

#define K_REX1	P34
#define K_REX2	P35
#define K_RESET	P40
#define K_DET	1
#define K_MAG	P54

#define KEY_DEL 10
#define KEY_SURE 11
#define KEY_ADMIN 12
#define KEY_UP 13
#define KEY_DOWN 14
#define KEY_BELL 15
#define KEY_TIMEOUT 99

//extern u8 DtmfData;
extern u16 CallOutTime;
extern u8 UnlockTime;

void WDT_config(void);

void delay(u16 t);
u8 strlen(u8 *pch);
u8 HBstrlen(u8 *pch);
//u8 strcmp(u8 *s,u8 *p);
//u8 *strchr(u8 *p,u8 c);
int lookforstr(u8 *srcBuf,u8 *detBuf);
void ClearBuf(u8 *buf,u16 len);
void HS_memcpy(u8 * dst,u8 * src,u16 len);
bool MemIsEmpty(u8 *buf,u16 len);
	
void GPIO_config(void);
void UART_config(void);
void Timer_config(void);
void Exti_config(void);
void PWM_config(void);

void PWMtask(void);

u8 KeyScan(void);
void SkeyScan(void);

void Unlock(bool sound);
void RelayOpen(u16 idx,u8 time);
void RelayCloaseAll(void);
void RelayTask(void);

#endif
