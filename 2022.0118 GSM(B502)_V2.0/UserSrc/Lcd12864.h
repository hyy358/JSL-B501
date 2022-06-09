#ifndef __LCD12864_H__
#define __LCD12864_H__

#include "Driver.h"

#define GRAPHIC_MODE


	#define cnCmdClr	0x01 
	#define cnCmdHome	0x02
	#define cnCmdEntryMode	0x04
	#define cnACInc	0x02
	#define cnACDec	0x00
	#define cnDisShift	0x01
	#define cnCmdDisOnOff	0x08
	#define cnDisOn		0x04
	#define cnDisOff	0x00
	#define cnDisCursorOn	0x02
	#define cnDisCurFlash	0x01
	#define cnCmdCursor	0x10
	#define cnDisMov	0x08
	#define cnCursorMov	0x00
	#define cnMoveRight	0x04
	#define cnMoveLeft	0x00
	#define cnCmdSetFun	0x20
	#define cnDataLen8	0x10
	#define cnBasicIns	0x00
	#define cnExtendIns	0x04
	#define cnCmdSetCAddr	0x40
	#define cnCmdSetDAddr	0x80
//---------------extended instruction----------------------	
	#define cnCmdStandBy	0x01
	#define cnCmdRamSelect	0x02
	#define cnEnCGRam	0x00
	#define cnCmdReverse	0x04
	#define cnCmdExtendFun	0x20
	#define cnEnGraphic	0x02
	#define	cnCmdSetIRam	0x40
	#define cnCmdSetGRam	0x80
/*====================================================================*/
/*--------------basic command----------------------------------------------*/
	#define	cnLCDEntryMode	 	cnCmdEntryMode|cnACInc
	#define cnLCDDisOff	 	cnCmdDisOnOff|cnDisOff
	#define cnLCDDisOn		cnCmdDisOnOff|cnDisOn
	#define cnLCDDisCursorOn		cnCmdDisOnOff|cnDisOn|cnDisCursorOn	//for debug
	#define cnLCDCursor		cnCmdCursor|cnCursorMov|cnMoveRight
	#define cnLCDSetDataLen8 	cnCmdSetFun|cnDataLen8
	#define cnLCDSetBasic	 	cnCmdSetFun|cnDataLen8|cnBasicIns
	#define cnLCDSetExtend		cnCmdSetFun|cnDataLen8|cnExtendIns
	#define cnLCDEnGraphic		cnCmdExtendFun|cnDataLen8|cnEnGraphic
	#define cnLCDGraphicOn		cnCmdExtendFun|cnDataLen8|cnExtendIns|cnEnGraphic
/*---------------extended command-------------------------------------*/	
	#define cnLCDEnCGRam	cnCmdRamSelect|cnEnCGRam
/********************************************************************************/

void Init_12864(void);
void LcdClear(void);
void LCD12864_PUTchar8x8(unsigned char row,unsigned char col,unsigned char count,unsigned char const *put);
void DisplayString(uint8_t X,uint8_t Y,const char *ptr);

#ifdef GRAPHIC_MODE

#define LCD_WIDTH 	128
#define LCD_HEIGHT	64
#define LCD_MAX_COL 16
#define ASC_CHR_HEIGHT 16
extern uint8_t mapGDRAM[LCD_HEIGHT][LCD_WIDTH/8];
void DisplayIco(uint8_t X,uint8_t Y,uint8_t ico);
void DisplayCsq(uint8_t X,uint8_t Y,uint8_t csq);
void SetFlash(uint8_t X,uint8_t Y);
void FlashTask(void);
#endif

#endif
