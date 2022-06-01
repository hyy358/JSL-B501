#include "Lcd12864.h"
#include "Driver.h"
#include "Audio.h"

#define delayNOP(); {_nop_();_nop_();_nop_();_nop_();};


bit lcd_busy()
{                          
	bit result;

	LCD12864_DATA = 0xFF;
	LCD12864_RS_L;
	LCD12864_WR_H;
	LCD12864_E_H;
	delayNOP();
	result = (LCD12864_DATA&0x80)==0x80?1:0;
	LCD12864_E_L;delayNOP();

	return(result); 
}
/*
* ��  �ܣ�дָ�LCD
* ��  ����Ҫд���ָ��
* ����ֵ����
*/
void lcd_wcmd(uint8_t cmd)
{           
	uint16_t timeout = 1000;
	while(lcd_busy() && (--timeout > 0));
	if(timeout == 0)
	{
		Beep_StartPlay(MP3_Key);
	}

	LCD12864_RS_L;
	LCD12864_WR_L;
	LCD12864_E_L;
	_nop_();
	_nop_(); 
	LCD12864_DATA = cmd;
	delayNOP();
	delayNOP();
	LCD12864_E_H;
	delayNOP();
	LCD12864_E_L;  
}

/*
* ��  �ܣ�LCDд����
* ��  ����Ҫд�������
* ����ֵ����
*/
void lcd_wdat(uint8_t dat)
{                          
	uint16_t timeout = 1000;
	while(lcd_busy() && (--timeout > 0));
	if(timeout == 0)
	{
		Beep_StartPlay(MP3_Key);
	}

	LCD12864_RS_H;
	LCD12864_WR_L;
	LCD12864_E_L;
	LCD12864_DATA = dat;
	delayNOP();
	delayNOP();
	LCD12864_E_H;
	delayNOP();
	LCD12864_E_L; 
}
void Init_12864(void)
{ 
	LCD12864_E_L;
	
	lcd_wcmd(0x30);      //����ָ�����
	//lcd_wcmd(0x34);      //����ָ�����
	delayNOP();
	lcd_wcmd(0x30);      //����ָ�����
	delayNOP();
	lcd_wcmd(0x0C);      //��ʾ�����ع��
	delayNOP();
	lcd_wcmd(0x01);      //���LCD����ʾ����
	delayNOP();
	lcd_wcmd(0x06);      //���LCD����ʾ����
	delayNOP();
}

void LcdClear(void)
{
	lcd_wcmd(0x01);
	delayNOP();
#ifdef GRAPHIC_MODE
	ClearBuf((uint8_t *)&mapGDRAM,sizeof(mapGDRAM));
#endif
	SetFlash(0xFF,0xFF);
}

#ifndef GRAPHIC_MODE
void DisplayString(u8 X,u8 Y,u8 *ptr)
{
  u8 i;
  lcd_wcmd(0x30);
  switch(X)
   {
     case 0:  Y+=0x80;break;
     case 1:  Y+=0x90;break;
     case 2:  Y+=0x88;break;
     case 3:  Y+=0x98;break;
     default: break;
   }
   lcd_wcmd(Y); // ��λ��ʾ��ʼ��ַ
	i = 0;
	while(ptr[i])
    { 
      lcd_wdat(ptr[i]);//��ʾ����ʱע����ֵ�������������ʾһ������
		i++;
		if(i%16 == 0)
		{
			X++;
			X %= 4;
			lcd_wcmd(0x30);
			switch(X)
		   {
			 case 0:  Y=0x80;break;
			 case 1:  Y=0x90;break;
			 case 2:  Y=0x88;break;
			 case 3:  Y=0x98;break;
			 default: break;
		   }
		   lcd_wcmd(Y);
		}
    }
	while((i%16) != 0)
    { 
		lcd_wdat(' ');
		i++;
    }
}
/*
void word(u8 X,u8 Y,u8 c)
{
	lcd_wcmd(0x30);
	switch(X)
	{
		case 0:  Y|=0x80;break;
		case 1:  Y|=0x90;break;
		case 2:  Y|=0x88;break;
		case 3:  Y|=0x98;break;
		default: break;
	}
	lcd_wcmd(Y);
	lcd_wdat(c);
}*/
#endif

#ifdef GRAPHIC_MODE

#include "AsciiFont.h"
#include "HebrewFont.h"

bool bRight2Left;
static uint8_t lcdRow = 0;	//VT12864Cһ��֧��4��������ʾ
static uint8_t lcdCol = 0;	//ÿ��֧��8�����ֻ�16��ASCII�ַ��������ʾ64��char
uint8_t mapGDRAM[LCD_HEIGHT][LCD_WIDTH/8];//GDRAM map, y=64, x=16*8
 
void BitBlt(uint8_t * pBmp)
{	// total 1024 bytes
	uint8_t i,j;
	const uint8_t * pNextFrame=pBmp+512;	//half

	for(i=0;i<(LCD_HEIGHT/2);i++)	//y
	{	//��ΪVT12864����ʾRAM�ǽ����ҵߵ��ģ�����Ҫ�����С��ߵ�ɨ�裨�ⲿ�ִ�����BmpToLcd�����У�
		lcd_wcmd(cnLCDGraphicOn);			//����ָ�
		lcd_wcmd(cnCmdSetGRam | i);		//����Y
		lcd_wcmd(cnCmdSetGRam | 0x00);	//����X
		lcd_wcmd(cnLCDSetBasic);
		for(j=0;j<16;j++)	//x=8*16=128
		{	//j=0~15ʱ����ϰ벿��128*32�����ݣ�j=15~31ʱ����°벿��128*32�����ݡ�
			lcd_wdat(*pBmp);
			pBmp++;
		#ifdef PWM_AUDIO
			WavReadTask();
		#endif
		}
		for(j=16;j<32;j++)	//x
		{	//j=0~15ʱ����ϰ벿��128*32�����ݣ�j=15~31ʱ����°벿��128*32�����ݡ�
			lcd_wdat(*pNextFrame);
			pNextFrame++;
		#ifdef PWM_AUDIO
			WavReadTask();
		#endif
		}
	}
}
//void DisplayFlA(u8 X,u8 Y)
//{	// total 1024 bytes
//	u8 i;
//	for(i=0;i<ASC_CHR_HEIGHT;i++)
//	{
//		mapGDRAM[i+ASC_CHR_HEIGHT*Y][X] = ~mapGDRAM[i+ASC_CHR_HEIGHT*Y][X];
//	}
////	BitBlt((u8 *)&mapGDRAM);
////	return;
//	for(i=0;i<ASC_CHR_HEIGHT;i++)	//y
//	{	//��ΪVT12864����ʾRAM�ǽ����ҵߵ��ģ�����Ҫ�����С��ߵ�ɨ�裨�ⲿ�ִ�����BmpToLcd�����У�
//		lcd_wcmd(cnLCDGraphicOn);			//����ָ�
//		lcd_wcmd(cnCmdSetGRam | (i+ASC_CHR_HEIGHT*(Y>1?(Y-2):Y)));		//����Y
//		lcd_wcmd(cnCmdSetGRam | X/2+(Y>1?8:0));	//����X
//		lcd_wcmd(cnLCDSetBasic);
//		//for(j=0;j<16;j++)	//x=8*16=128
//		{	//j=0~15ʱ����ϰ벿��128*32�����ݣ�j=15~31ʱ����°벿��128*32�����ݡ�
//			
//			if(X%2)
//			{
//				lcd_wdat(mapGDRAM[i+ASC_CHR_HEIGHT*Y][X-1]);
//				lcd_wdat(mapGDRAM[i+ASC_CHR_HEIGHT*Y][X]);
//			}
//			else
//			{
//				lcd_wdat(mapGDRAM[i+ASC_CHR_HEIGHT*Y][X]);
//			}
//		}
//	}
//}

void DisplayFl(uint8_t X,uint8_t Y,bool e)
{

	lcd_wcmd(cnLCDGraphicOn);			//����ָ�
	lcd_wcmd(cnCmdSetGRam | (14+ASC_CHR_HEIGHT*(Y>1?(Y-2):Y)));		//����Y
	lcd_wcmd(cnCmdSetGRam | X/2+(Y>1?8:0));	//����X
	lcd_wcmd(cnLCDSetBasic);	
	if(X%2)
	{
		lcd_wdat(mapGDRAM[14+ASC_CHR_HEIGHT*Y][X-1]);
	}
	lcd_wdat(e?0xFF:mapGDRAM[14+ASC_CHR_HEIGHT*Y][X]);

}

#define HEBREW_START 0xD790
#define HEBREW_END 0xD7AB

bool CheckUnicode(uint8_t *pInfo)
{
	while((*pInfo != '\0') && (*pInfo != '\n'))
	{
		if(*pInfo > 0x80)
		{
			return true;
		}
		pInfo++;
	}
	return false;
}
void LCDAddRow(void)
{
	lcdRow++;
	lcdRow %= 4;
	lcdCol=0;
}
void WriteLCD(uint8_t *pInfo)	//д���ݵ���Ļ����ʾ
{

	uint8_t x,y;
	uint8_t c;
//	u8 utf[4];
	const typFNT_Ascii *Font;
	uint16_t iUnicode;
	bRight2Left = CheckUnicode(pInfo);
	
	if(pInfo == NULL) return;
	
	while((*pInfo != '\0') && (*pInfo != '\n'))
	{	//���ǽ����ַ�
//		dFont = NULL;
		c = *pInfo;

		lcdCol++;
		if(lcdCol > LCD_MAX_COL){		//Ҫ����
//			if(g_bShowNameList){
//				break;	//only display 1 line
//			}else{
//				LCDAddRow();
//			}
			LCDAddRow();
		}
		
		Font = &AsciiFont['?'-' '];	//unknow char
		if(c >= ' ')
		{	//can display
			if(c <= 0x80){ //ASCII
				Font = &AsciiFont[c - ' '];
			}
			else
			{	//UTF-8
				pInfo++;
//				utf[0] = c;
//				utf[1] = *pInfo;
				//if((utf[0] >= 0xC0) && (utf[0] <= 0xDF) && (utf[1] >= 0x80) && (utf[1] < 0xC0))
				{	//2 bytes chars in UTF-8
//					utf[0] &= 0x1F;
//					utf[1] &= 0x3F;
//					iUnicode = utf[0] << 6;
//					iUnicode += utf[1];
					iUnicode = c;
					iUnicode <<= 8;
					iUnicode &= 0xFF00;
					iUnicode += *pInfo;
					if((iUnicode >= HEBREW_START) && (iUnicode <= HEBREW_END)){	//Hebrew
						Font=&HebrewFont[iUnicode-HEBREW_START];
					}
//					else if((iUnicode >= 0x0600) && (iUnicode <= 0x06FF)){	//Arab
//						Font=&ArabFont[iUnicode-0x0600];
//					}
				}
			}
		}
		if(lcdCol != 0) 
			x = lcdCol-1;
		else{
			x = 0;
			lcdCol = 1;
		}

		if(bRight2Left){
			x = (LCD_WIDTH/8-1)-x;
		}
		for(y=0;y<ASC_CHR_HEIGHT;y++)
		{
			mapGDRAM[y+ASC_CHR_HEIGHT*lcdRow][x] = Font->Msk[y];
		}
		pInfo++;
	};	//while

}

void WriteLCDico(uint8_t Row,uint8_t Col,uint8_t ico,uint8_t mask)	//д˽ߝսǁĻʏДʾ
{

	uint8_t y;
	const typFNT_Ascii *Font;
	Font = &IcoFont[ico];
	for(y=0;y<ASC_CHR_HEIGHT;y++)
	{
		mapGDRAM[y+ASC_CHR_HEIGHT*Row][Col] = Font->Msk[y]&mask;
	}
}
#include "stdio.h"
void DisplayString(uint8_t X,uint8_t Y,const char *ptr)
{
	lcdRow = X;
	lcdCol = Y;
	WriteLCD((uint8_t *)ptr);
	BitBlt((uint8_t *)&mapGDRAM);
}

void DisplayIco(uint8_t X,uint8_t Y,uint8_t ico)
{
	WriteLCDico(X,Y,ico,0xFF);
	BitBlt((uint8_t *)&mapGDRAM);
}
//void DisplayCsq(u8 X,u8 Y,u8 csq)
//{
//	u8 i;
//	u8 msk = 0;
//	csq /= 5;
//	if(csq > 5)
//	{
//		csq = 5;
//	}
//	for(i=1;i<csq;i++)
//	{
//		msk >>= 2;
//		msk |= 0xC0;
//	}
//	WriteLCDico(X,Y,4,(csq > 0)?0xff:0);
//	WriteLCDico(X,Y+1,5,msk);
//	BitBlt((u8 *)&mapGDRAM);
//}

bool flg = false;
uint16_t FlashTime = 0;
uint8_t Fl_X = 0xFF;
uint8_t Fl_Y = 0xFF;
void SetFlash(uint8_t X,uint8_t Y)
{
	Fl_X = Y;
	Fl_Y = X;
}
void FlashTask(void)
{
	if((Fl_X == 0xFF) || (Fl_Y == 0xFF))
	{
		return;
	}
	if(++FlashTime == 300)
	{
		FlashTime = 0;
		DisplayFl(Fl_X,Fl_Y,flg);
		flg = !flg;
	}
}

#endif	
