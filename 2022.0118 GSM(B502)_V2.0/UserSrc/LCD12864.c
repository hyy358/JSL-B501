#include "Lcd12864.h"
#include "Driver.h"
#include "Audio.h"

#define delayNOP(); {_nop_();_nop_();};


bool lcd_busy()
{                          
	bool result;

	LCD12864_DATA = 0xFF;
	LCD12864_RS_L;
	LCD12864_WR_H;
	LCD12864_E_H;
	delayNOP();
	result = (LCD12864_DATA&0x80)==0x80?true:false;
	LCD12864_E_L;delayNOP();
	
	return(result); 
}
/*
* 功  能：写指令到LCD
* 参  数：要写入的指令
* 返回值：无
*/
void lcd_wcmd(uint8_t cmd)
{                          
	while(lcd_busy());

	LCD12864_RS_L;
	LCD12864_WR_L;
	LCD12864_E_L;
	_nop_();
	_nop_(); 
	LCD12864_DATA = cmd;
	delayNOP();
//	delayNOP();
	LCD12864_E_H;
	delayNOP();
	LCD12864_E_L;  
}

/*
* 功  能：LCD写数据
* 参  数：要写入的数据
* 返回值：无
*/
void lcd_wdat(uint8_t dat)
{                          
	while(lcd_busy());

	LCD12864_RS_H;
	LCD12864_WR_L;
	LCD12864_E_L;
	LCD12864_DATA = dat;
	delayNOP();
//	delayNOP();
	LCD12864_E_H;
	delayNOP();
	LCD12864_E_L; 
}
void Init_12864(void)
{ 
	LCD12864_E_L;
	
	lcd_wcmd(0x30);      //基本指令操作
	//lcd_wcmd(0x34);      //扩充指令操作
	delayNOP();
	lcd_wcmd(0x30);      //基本指令操作
	delayNOP();
	lcd_wcmd(0x0C);      //显示开，关光标
	delayNOP();
	lcd_wcmd(0x01);      //清除LCD的显示内容
	delayNOP();
	lcd_wcmd(0x06);      //清除LCD的显示内容
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
   lcd_wcmd(Y); // 定位显示起始地址
	i = 0;
	while(ptr[i])
    { 
      lcd_wdat(ptr[i]);//显示汉字时注意码值，连续两个码表示一个汉字
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
static uint8_t lcdRow = 0;	//VT12864C一共支持4行中文显示
static uint8_t lcdCol = 0;	//每行支持8个汉字或16个ASCII字符，最多显示64个char
uint8_t mapGDRAM[LCD_HEIGHT][LCD_WIDTH/8];//GDRAM map, y=64, x=16*8
 
void BitBlt(uint8_t * pBmp)
{	// total 1024 bytes
	uint8_t i,j;
	const uint8_t * pNextFrame=pBmp+512;	//half

	for(i=0;i<(LCD_HEIGHT/2);i++)	//y
	{	//因为VT12864的显示RAM是交叉且颠倒的，所以要“隔行”颠倒扫描（这部分处理在BmpToLcd程序中）
		lcd_wcmd(cnLCDGraphicOn);			//扩充指令集
		lcd_wcmd(cnCmdSetGRam | i);		//设置Y
		lcd_wcmd(cnCmdSetGRam | 0x00);	//设置X
		lcd_wcmd(cnLCDSetBasic);
#ifdef PWM_AUDIO
		if(WavNeedRead)WavReadTask();
#endif	
		for(j=0;j<16;j++)	//x=8*16=128
		{	//j=0~15时输出上半部分128*32的数据，j=15~31时输出下半部分128*32的数据。
			lcd_wdat(*pBmp);
			pBmp++;	
		}
		for(j=16;j<32;j++)	//x
		{	//j=0~15时输出上半部分128*32的数据，j=15~31时输出下半部分128*32的数据。
			lcd_wdat(*pNextFrame);
			pNextFrame++;
		}

	}
}

void DisplayFl(uint8_t X,uint8_t Y,bool e)
{

	lcd_wcmd(cnLCDGraphicOn);			//扩充指令集
	lcd_wcmd(cnCmdSetGRam | (15+ASC_CHR_HEIGHT*(Y>1?(Y-2):Y)));		//设置Y
	lcd_wcmd(cnCmdSetGRam | X/2+(Y>1?8:0));	//设置X
	lcd_wcmd(cnLCDSetBasic);	
	if(X%2)
	{
		lcd_wdat(mapGDRAM[15+ASC_CHR_HEIGHT*Y][X-1]);
	}
	lcd_wdat(e?0xFF:mapGDRAM[15+ASC_CHR_HEIGHT*Y][X]);

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
void WriteLCD(uint8_t *pInfo)	//写数据到屏幕上显示
{

	uint8_t x,y;
	uint8_t c;
//	u8 utf[4];
	const typFNT_Ascii *Font;
	uint16_t iUnicode;
	bRight2Left = CheckUnicode(pInfo);
	
	if(pInfo == NULL) return;
	
	while((*pInfo != '\0') && (*pInfo != '\n'))
	{	//不是结束字符
//		dFont = NULL;
		c = *pInfo;

		lcdCol++;
		if(lcdCol > LCD_MAX_COL){		//要换行
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

void WriteLCDico(uint8_t Row,uint8_t Col,uint8_t ico,uint8_t mask)	//写私邼战莵幕蕪袛示
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
void DisplayCsq(uint8_t X,uint8_t Y,uint8_t csq)
{
	uint8_t i;
	uint8_t msk = 0;
	csq /= 5;
	if(csq > 5)
	{
		csq = 5;
	}
	for(i=1;i<csq;i++)
	{
		msk >>= 2;
		msk |= 0xC0;
	}
	WriteLCDico(X,Y,4,(csq > 0)?0xff:0);
	WriteLCDico(X,Y+1,5,msk);
	BitBlt((uint8_t *)&mapGDRAM);
}

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
