#ifndef __DRIVER_H__
#define __DRIVER_H__
#include "config.h"

#define SYS_LED P01

#define LCD12864_E_H	{P52 = 1;}
#define LCD12864_E_L	{P52 = 0;}
#define LCD12864_WR_H	{P53 = 1;}
#define LCD12864_WR_L	{P53 = 0;}
#define LCD12864_RS_H	{P44 = 1;}
#define LCD12864_RS_L	{P44 = 0;}
#define LCD12864_DATA	P1

#define KEY_P P2

#define GSM_PWREN_H	{P70 = 1;}
#define GSM_PWREN_L	{P70 = 0;}
#define GSM_PWRKEY_H	{P41 = 1;}
#define GSM_PWRKEY_L	{P41 = 0;}

#define PA_PWR_H	{P00 = 1;}
#define PA_PWR_L	{P00 = 0;}

#define IIC_WP_H	{P71 = 1;}
#define IIC_WP_L	{P71 = 0;}
#define IIC_CLK_H	{P72 = 1;}
#define IIC_CLK_L	{P72 = 0;}
#define IIC_DAT_H	{P73 = 1;}
#define IIC_DAT_L	{P73 = 0;}
#define IIC_DAT		P73

#define RELAY_1_ON	{P43 = 1;}
#define RELAY_1_OFF	{P43 = 0;}
#define RELAY_2_ON	{P04 = 1;}
#define RELAY_2_OFF	{P04 = 0;}

#define K_REX1	P06
#define K_REX2	P05
#define K_RESET	P40
#define K_DET	P54
#define K_MAG	P55

//#define RFID_SHD_H	{P35 = 1;}
//#define RFID_SHD_L	{P35 = 0;}
#define RFID_DAT0	P33
#define RFID_DAT1	P34


#define CAMERA_ON	{P61 = 1;}
#define CAMERA_OFF	{P61 = 0;}

#define SPI_FLASH_CS_EN {P74 = 0;}
#define SPI_FLASH_CS_DISEN {P74 = 1;}

#define KEY_DEL 10
#define KEY_SURE 11
#define KEY_ADMIN 12
#define KEY_UP 13
#define KEY_DOWN 14
#define KEY_BELL 15
#define KEY_TIMEOUT 99

extern uint8_t DtmfData;

void delay(uint16_t t);
uint8_t strlen(uint8_t *pch);
uint8_t HBstrlen(uint8_t *pch);
int lookforstr(uint8_t *srcBuf,uint8_t *detBuf);
void ClearBuf(uint8_t *buf,uint16_t len);
void HS_memcpy(uint8_t * dst,uint8_t * src,uint16_t len);
uint16_t HS_strcpy(uint8_t * dst,uint8_t * src);
bool MemIsEmpty(uint8_t *buf,uint16_t len);
	
void GPIO_config(void);
void UART_config(void);
void Timer_config(void);
void Exti_config(void);

uint8_t KeyScan(void);
void SkeyScan(void);

void Unlock(uint8_t idx,uint8_t time,bool sound);
void RelayTask(void);

#endif
