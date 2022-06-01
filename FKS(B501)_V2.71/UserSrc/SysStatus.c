#include "config.h"
#include "Driver.h"
#include "SysStatus.h"
#include "Lcd12864.h"
#include "Eeprom.h"
#include "NameList.h"
#include "UnlockPassword.h"
#include "SysString.h"
#include "Card.h"
#include "stdio.h"
#include "Audio.h"

SysStatusType SysStatus = SYS_IDLE;
UsrParaType UsrPara = {0,0,0,0};
SysParaType SysPara;
u16 NameListIdx = 0xFFFF;
u16 IdleTime = 0;
uint8_t Volum = 3;

NameItemType NameItemCall;
u8 CallTleIdx = 0;
void InitSysPara(bool rst)
{
	readBuf_at24c02(SYS_PARA_ADDR, (u8 *)&SysPara,sizeof(SysPara));
	if((SysPara.InitFlag != PARA_INIT_FLAG) || rst)
	{
		SysPara.InitFlag = PARA_INIT_FLAG;
//		SysPara.TalkTime = 30;
//		SysPara.RingInTs = 3;
		SysPara.RingOutTs = 2;
		SysPara.ToneEn = 1;
		SysPara.CameraOn = 0;
		SysPara.RealyTime = 10;
//		SysPara.UnlockKey = 5;
		SysPara.TalkVolum = 3;
		SysPara.SysPsw = 666666;
		writeBuf_at24c02(SYS_PARA_ADDR, (u8 *)&SysPara,sizeof(SysPara));
	}
	
//	printf("TalkTime: %u\r\n",SysPara.TalkTime);
//	printf("RingInTs: %bd\r\n",SysPara.RingInTs);
	printf("RingOutTs: %bd\r\n",SysPara.RingOutTs);
	printf("ToneEn: %bd\r\n",SysPara.ToneEn);
	printf("CameraOn: %bd\r\n",SysPara.CameraOn);
	printf("RealyTime: %bd\r\n",SysPara.RealyTime);
//	printf("UnlockKey: %bd\r\n",SysPara.UnlockKey);
	printf("NameCount: %u\r\n",UsrPara.NameListCount);
	printf("PswCount: %u\r\n",UsrPara.PswCount);
	printf("CardCount: %u\r\n",UsrPara.CardCount);
	printf("TalkVolum: %bd\r\n",SysPara.TalkVolum);
//	printf("SysPsw: %lu\r\n",SysPara.SysPsw);
	
	
	if(SysPara.TalkVolum > 5)
	{
		SysPara.TalkVolum = 3;
	}
	Volum = SysPara.TalkVolum;
	
	if(SysPara.CameraOn)
	{
		CAMERA_ON;
	}
	else
	{
		CAMERA_OFF;
	}
}

void InitUserPara(void)
{
	readBuf_at24c02(USER_PARA_ADDR, (u8 *)&UsrPara,sizeof(UsrPara));
	if(UsrPara.InitFlag != PARA_INIT_FLAG)
	{
		UsrPara.InitFlag = PARA_INIT_FLAG;
		UsrPara.NameListCount = 0;
		UsrPara.PswCount = 0;
		UsrPara.CardCount = 0;
		writeBuf_at24c02(USER_PARA_ADDR, (u8 *)&UsrPara,sizeof(UsrPara));
	}
	InitNameList();
	InitPsw();
	InitCard();
}

u16 ReadRoomByIdx(u16 idx,EromReadType type)
{
	u16 room;
	u16 addr = (u16)idx;
	switch(type)
	{
		case NAME_LIST:
			addr *= sizeof(NameItemType);
			addr += NAMELIST_ADDR;
			break;
		case UNLOCK_PSW:
			addr *= sizeof(PswItemType);
			addr += USER_PSW_ADDR;
			break;
		case CARD_LIST:
			addr *= sizeof(CardItemType);
			addr += USER_CARD_ADDR;
			break;
	}
	
	
	readBuf_at24c02(addr, (u8 *)&room,sizeof(room));
	return room;
}

u16 GetIdxByRoom(u16 room,EromReadType type)
{
	u16 i =0;
	u16 DstRoom;
	u16 iMax;
	switch(type)
	{
		case NAME_LIST:
			iMax = UsrPara.NameListCount;
			break;
		case UNLOCK_PSW:
			iMax = UsrPara.PswCount;
			break;
		case CARD_LIST:
			iMax = UsrPara.CardCount;
			break;
	}
	while(i<iMax)
	{
		DstRoom = ReadRoomByIdx(i,type);
		if(room == DstRoom)
		{
			return i;
		}
		else if(DstRoom == 0xFFFF)
		{
			break;
		}
		i++;
	}
	return 0xFFFF;
}

u16 InputRoom = 0;
u8 InputIdx = 0;

void SysIdleCon(void)
{
	SysStatus = SYS_IDLE;
	NameListIdx = 0xFFFF;
	IdleTime = 0;
	ShowMain(true);
}
void SysSettingCon(void)
{
	SysStatus = SYS_SETTING;
	LcdClear();
	DisplayString(1,1,StrTabSysSet);
	DisplayString(3,1,StrTabSysSet1);

	DisplayIco(3,6,2);
	InputRoom = 0;
	InputIdx = 0;
}

void ShowInputRoom(void)
{
	u8 Disbuf[24];
	LcdClear();
//	if(InputRoom > 0)
//	{
//		sprintf(Disbuf,"  %s%u",StrTabRoom,InputRoom);
//	}
//	else
//	{
//		sprintf(Disbuf,"  %s",StrTabRoom);
//	}
//	DisplayString(1,0,Disbuf);
	DisplayString(1,0,StrTabRoom);
	if(InputRoom > 0)
	{
		sprintf(Disbuf,"%u",InputRoom);
		DisplayString(1,(15-HBstrlen(StrTabRoom)-strlen(Disbuf)),Disbuf);
	}
}

void InputRoomCon(u8 key)
{
	SysStatus = SYS_INPUT_ROOM;
	InputRoom = key;
	InputIdx = 1;
	if(InputRoom == 0)
	{
		InputIdx = 0;
	}
	ShowInputRoom();
	ClearBuf((u8 *)&NameItemCall,sizeof(NameItemCall));
	MP3_StartPlay(MP3_InputRoom);
}

void SysInputRoom(u8 key)
{
	switch(key)
	{
		case KEY_BELL:
			if(InputRoom > 0)
			{
				if(GetNameItemByRoom(InputRoom,&NameItemCall) != 0xFFFF)
				{
					CallTleIdx = 0;
					if(CallByNameItem(&NameItemCall))
					{
						SysStatus = SYS_CALL;
						CallOutTime = SysPara.RingOutTs;
					}
					else// if(NameItemCall.Tel == 0)
					{
						ShowErr();
						IdleTime = 3;
					}
//					else
//					{
//						ShowGsmErr();
//						IdleTime = 3;
//					}
					return;
				}
			}
			
			ShowRoomErr();
			MP3_StartPlay(MP3_RoomErr);
			IdleTime = 3;
			InputRoom = 0;
			InputIdx = 0;
			break;
		case KEY_DEL:
			if(InputIdx > 0)
			{
				InputIdx--;
				InputRoom /= 10;
				ShowInputRoom();
			}
			else
			{
			#ifdef PWM_AUDIO
				MP3_Stop();
			#endif
				SysIdleCon();
			}
			break;
	}
	if((key >= 0) && (key <= 9))
	{
		if(InputIdx < 4)
		{
			InputIdx++;
			InputRoom *= 10;
			InputRoom += key;
			if(InputRoom == 0)
			{
				InputIdx = 0;
			}
			ShowInputRoom();
		}
	}
}

u32 InputPsw = 0;
void ShowInputPsw(void)
{
	u8 Disbuf[30],i;
	LcdClear();
	if(SysStatus == SYS_INPUT_SYSPSW)
	{
		DisplayString(1,0,StrTabSysPsw);
	}
//	if(InputPsw > 0)
//	{
//		sprintf(Disbuf," %s%lu",StrTabPsw1,InputPsw);
//	}
//	else
//	{
//		sprintf(Disbuf," %s",StrTabPsw1);
//	}
	
	sprintf(Disbuf,"%s%",StrTabPsw1);
	for(i=0;i<InputIdx;i++)
	{
		Disbuf[strlen(StrTabPsw1)+i] = '*';
	}
	Disbuf[strlen(StrTabPsw1)+i] = 0;
			
	DisplayString(2,0,Disbuf);
	#ifdef LANGUAGE_HB	
		SetFlash(2,(9-InputIdx));
	#else
		SetFlash(2,strlen(Disbuf));
	#endif
}
void InputSysPswCon(void)
{
	SysStatus = SYS_INPUT_SYSPSW;
	InputPsw = 0;
	InputIdx = 0;
	ShowInputPsw();
	MP3_StartPlay(MP3_InputPsw);
}

void InputSysPsw(u8 key)
{
	switch(key)
	{
		case KEY_SURE:
			if((InputPsw > 0) && (InputIdx == 6))
			{
				if(InputPsw == SysPara.SysPsw)
				{
					SysSettingCon();
					return;
				}
			}
			
			ShowErr();
			MP3_StartPlay(MP3_PwdErr);
			InputPsw = 0;
			InputIdx = 0;
			break;
		case KEY_DEL:
			if(InputIdx > 0)
			{
				InputIdx--;
				InputPsw /= 10;
				ShowInputPsw();
			}
			else
			{
				
#ifdef PWM_AUDIO
				MP3_Stop();
#endif
				SysIdleCon();
			}
			break;
		case KEY_BELL:
#ifdef PWM_AUDIO
				MP3_Stop();
#endif
			SysIdleCon();
			break;
	}
	if((key >= 0) && (key <= 9))
	{
		if(InputIdx < 6)
		{
			InputIdx++;
			InputPsw *= 10;
			InputPsw += key;
			ShowInputPsw();
		}
	}
}

void SysIdleKey(u8 key)
{
	switch(key)
	{
		case KEY_DEL:
//			if(NameListIdx == 0xFFFF)
//			{
//				InputSysPswCon();
//			}
//			else
			{
				SysIdleCon();
			}
			break;
		case KEY_SURE:
			InputPswCon();
			break;
		case KEY_ADMIN:
			InputSysPswCon();
			break;
		case KEY_UP:
			if(UsrPara.NameListCount > 0)
			{
				if(NameListIdx < (UsrPara.NameListCount-1))
				{
					NameListIdx++;
				}
				else
				{
					NameListIdx = 0;
				}
				ShowNameByIdx(NameListIdx);
			}
			break;
		case KEY_DOWN:
			if(UsrPara.NameListCount > 0)
			{
				if(NameListIdx < UsrPara.NameListCount)
				{
					if(NameListIdx > 0)
					{
						NameListIdx--;
					}
					else
					{
						NameListIdx = UsrPara.NameListCount-1;
					}
				}
				else
				{
					NameListIdx = UsrPara.NameListCount-1;
				}
				ShowNameByIdx(NameListIdx);
			}
			break;
		case KEY_BELL:
			if(NameListIdx < UsrPara.NameListCount)
			{
				CallTleIdx = 0;
				ReadNameItemByIdx(NameListIdx,&NameItemCall);
				if(CallByNameItem(&NameItemCall))
				{
					SysStatus = SYS_CALL;
					CallOutTime = SysPara.RingOutTs;
				}
				else
				{
					ShowErr();
					IdleTime = 3;
				}
//				else
//				{
//					ShowGsmErr();
//					IdleTime = 3;
//				}
			}
			else
			{
				InputRoomCon(0);
			}
			break;
	}
	if((key > 0) && (key <= 9))
	{
		InputRoomCon(key);
	}
}

void SysSettingKey(u8 key)
{
//	if((key >= 0) && (key <= 9))
//	{
//		InputRoom = key;
//	}
//	else if(key == KEY_BELL)
//	{
//		SysIdleCon();
//	}
//	else if(key == KEY_DEL)
//	{
//		SysIdleCon();
//	}
//	else if(key == KEY_SURE)
//	{
//		switch(InputRoom)
//		{
//			case 11:
//				EditSysPswCon();
//				break;
//			case 1:
//				NameSetCon();
//				break;
//			case 2:
//				PswSetCon();
//				break;
//			case 3:
//				CardSetCon();
//				break;
//			case 4:
//			case 5:
//			case 6:
//			case 7:
////			case 8:
////			case 9:
////			case 10:
//				SubSetCon(SYS_SET_TALKTIME+InputRoom-4);
//				break;
//		}
//	}
/*
	switch(key)
	{
		case KEY_DEL:
		case KEY_BELL:
			SysIdleCon();
			break;
		case 8:
			EditSysPswCon();
			break;
		case 1:
			NameSetCon();
			break;
		case 2:
			PswSetCon();
			break;
		case 3:
			CardSetCon();
			break;
		case 4:
		case 5:
		case 6:
		case 7:
//		case 8:
//		case 9:
			SubSetCon(SYS_SET_TONE+key-4);
			break;
	}
*/
	if((key >= 0) && (key <= 9))
	{
		if(InputIdx < 2)
		{
			InputIdx++;
			InputRoom *= 10;
			InputRoom += key;
			if(InputRoom > 13)
			{
				InputRoom = key;
				InputIdx = 1;
			}
		}
		else
		{
			InputRoom = key;
			InputIdx = 1;
		}
	}
	else if(key == KEY_BELL)
	{
		SysIdleCon();
	}
	else if(key == KEY_DEL)
	{
		SysIdleCon();
	}
	else if(key == KEY_SURE)
	{
		if((InputRoom >= 4) && (InputRoom<12))
		{
			SubSetCon(SYS_SET_TALKTIME+InputRoom-4);
			return;
		}
		switch(InputRoom)
		{
			case 12:
				EditSysPswCon();
				break;
			case 1:
				NameSetCon();
				break;
			case 2:
				PswSetCon();
				break;
			case 3:
				CardSetCon();
				break;
			case 13:
				SubSetCon(SYS_SET_TALKVOLUM);
				break;
		/*	case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				SubSetCon(SYS_SET_TALKTIME+InputRoom-4);
				break;*/
		}
	}
}

void SysTalkKey(u8 key)
{
	switch(key)
	{
		case KEY_DEL:
			RelayCloaseAll();
			SysIdleCon();
			break;
	}
}

u8 InputLen = 4;
u16 InputMax = 300;
void ShowSubPara(void)
{
	u8 Disbuf[24];
	LcdClear();
	switch(SysStatus)
	{
//		case SYS_SET_TALKTIME:
//			sprintf(Disbuf,"%s",StrTabTalkTime);
//			break;
//		case SYS_SET_RINGIN:
//			sprintf(Disbuf,"%s",StrTabRingTime);
//			break;
		case SYS_SET_RINGOUT:
			sprintf(Disbuf,"%s",StrTabCallTime);
			break;
		case SYS_SET_TONE:
			sprintf(Disbuf,"%s",StrTabTone);
			break;
		case SYS_SET_CAMERA:
			sprintf(Disbuf,"%s",StrTabCamrea);
			if(InputRoom != 0)
			{
				CAMERA_ON;
			}
			else
			{
				CAMERA_OFF;
			}
			break;
		case SYS_SET_RELAY:
			sprintf(Disbuf,"%s",StrTabRelay);
			break;
//		case SYS_SET_UNLOCKKEY:
//			sprintf(Disbuf,"%s",StrTabUnlockKey);
//			break;
		case SYS_SET_TALKVOLUM:
			sprintf(Disbuf,"%s",StrTabTalkValum);
			break;
	}
	if((SysStatus == SYS_SET_TONE) || (SysStatus == SYS_SET_CAMERA))
//	if(SysStatus == SYS_SET_CAMERA)
	{
		//sprintf(Disbuf,"%s%s",Disbuf,((InputRoom==0)?StrTabOff:StrTabOn));
		DisplayString(1,0,Disbuf);
		DisplayString(2,5,((InputRoom==1)?StrTabOn:StrTabOff));
		return;
	}
//	else if(InputRoom > 0)
//	{
//		sprintf(Disbuf,"%s%u",Disbuf,InputRoom);
//	}
	
	DisplayString(1,0,Disbuf);
	if((InputRoom > 0) && (SysStatus != SYS_SET_CAMERA))
	{
		sprintf(Disbuf,"%u",InputRoom);
		DisplayString(2,8,Disbuf);
	//	SetFlash(2,8+strlen(Disbuf));
	}
}
	
void SubSetCon(SysStatusType type)
{
	u16 tmp;
	
	switch(type)
	{
//		case SYS_SET_TALKTIME:
//			InputLen = 3;
//			InputMax = 300;
//			InputRoom = SysPara.TalkTime;
//			break;
//		case SYS_SET_RINGIN:
//			InputLen = 2;
//			InputMax = 15;
//			InputRoom = SysPara.RingInTs;
//			break;
		case SYS_SET_RINGOUT:
			InputLen = 2;
			InputMax = 90;
			InputRoom = SysPara.RingOutTs;
			break;
		case SYS_SET_TONE:
			InputLen = 1;
			InputMax = 1;
			InputRoom = SysPara.ToneEn;
			break;
		case SYS_SET_CAMERA:
			InputLen = 1;
			InputMax = 1;
			InputRoom = SysPara.CameraOn;
			break;
		case SYS_SET_RELAY:
			InputLen = 2;
			InputMax = 99;
			InputRoom = SysPara.RealyTime;
			break;
//		case SYS_SET_UNLOCKKEY:
//			InputLen = 1;
//			InputMax = 9;
//			InputRoom = SysPara.UnlockKey;
//			break;
		case SYS_SET_TALKVOLUM:
			InputLen = 1;
			InputMax = 5;
			InputRoom = SysPara.TalkVolum;
			break;
		default:
			return;
	}
	
	SysStatus = type;
	InputIdx = 0;
	tmp =InputRoom;
	while(tmp > 0)
	{
		InputIdx++;
		tmp /= 10;
	}
//	if((SysStatus == SYS_SET_TONE) || (SysStatus == SYS_SET_CAMERA) || (SysStatus == SYS_SET_UNLOCKKEY))
	if((SysStatus == SYS_SET_TONE) || (SysStatus == SYS_SET_CAMERA) || (SysStatus == SYS_SET_TALKVOLUM))
	{
		InputIdx = 1;
	}
	ShowSubPara();
}

void SaveSubPara(SysStatusType type,u16 dat)
{
	switch(type)
	{
//		case SYS_SET_TALKTIME:
//			SysPara.TalkTime = dat;
//			writeBuf_at24c02(SYS_PARA_ADDR+((u16)((u32)(&SysPara.TalkTime)-(u32)(&SysPara))), (u8 *)&SysPara.TalkTime,sizeof(SysPara.TalkTime));
//			break;
//		case SYS_SET_RINGIN:
//			SysPara.RingInTs = (u8)dat;
//			writeBuf_at24c02(SYS_PARA_ADDR+((u16)((u32)(&SysPara.RingInTs)-(u32)(&SysPara))), (u8 *)&SysPara.RingInTs,sizeof(SysPara.RingInTs));
//			break;
		case SYS_SET_RINGOUT:
			SysPara.RingOutTs = (u8)dat;
			writeBuf_at24c02(SYS_PARA_ADDR+((u16)((u32)(&SysPara.RingOutTs)-(u32)(&SysPara))), (u8 *)&SysPara.RingOutTs,sizeof(SysPara.RingOutTs));
			break;
		case SYS_SET_TONE:
			SysPara.ToneEn = ((dat==0)?0:1);
			writeBuf_at24c02(SYS_PARA_ADDR+((u16)((u32)(&SysPara.ToneEn)-(u32)(&SysPara))), (u8 *)&SysPara.ToneEn,sizeof(SysPara.ToneEn));
			break;
		case SYS_SET_CAMERA:
			SysPara.CameraOn = ((dat==0)?0:1);
			writeBuf_at24c02(SYS_PARA_ADDR+((u16)((u32)(&SysPara.CameraOn)-(u32)(&SysPara))), (u8 *)&SysPara.CameraOn,sizeof(SysPara.CameraOn));
			break;
		case SYS_SET_RELAY:
			SysPara.RealyTime = (u8)dat;
			writeBuf_at24c02(SYS_PARA_ADDR+((u16)((u32)(&SysPara.RealyTime)-(u32)(&SysPara))), (u8 *)&SysPara.RealyTime,sizeof(SysPara.RealyTime));
			break;
//		case SYS_SET_UNLOCKKEY:
//			SysPara.UnlockKey = (u8)dat;
//			writeBuf_at24c02(SYS_PARA_ADDR+((u16)((u32)(&SysPara.UnlockKey)-(u32)(&SysPara))), (u8 *)&SysPara.UnlockKey,sizeof(SysPara.UnlockKey));
//			break;
		case SYS_SET_TALKVOLUM:
			if(dat > 0)
			{
				SysPara.TalkVolum = (uint8_t)dat;
				writeBuf_at24c02(SYS_PARA_ADDR+((u16)((u32)(&SysPara.TalkVolum)-(u32)(&SysPara))), (u8 *)&SysPara.TalkVolum,sizeof(SysPara.TalkVolum));
				Volum = SysPara.TalkVolum;
			}
			break;
	}
	ShowSaveOK();
}

void SysSubSetKey(u8 key)
{
	if(InputIdx == 99)
	{
		SysSettingCon();
		return;
	}
	switch(key)
	{
		case KEY_DEL:
			if((SysStatus == SYS_SET_TONE) || (SysStatus == SYS_SET_CAMERA) || (InputIdx == 0))
//			if((SysStatus == SYS_SET_CAMERA) || (InputIdx == 0))
			{
				if(SysPara.CameraOn)
				{
					CAMERA_ON;
				}
				else
				{
					CAMERA_OFF;
				}
				SysSettingCon();
				return;
			}
			
			InputIdx--;
			InputRoom /= 10;
			ShowSubPara();
			break;
		case KEY_BELL:
			SysSettingCon();
			return;
		case KEY_SURE:
			if(InputIdx > 0)
			{
				SaveSubPara(SysStatus,InputRoom);
				InputIdx = 99;
			}
			else
			{
				SysSettingCon();
			}
			return;
	}
	if((key >= 0) && (key <= 9))
	{
		//if((SysStatus == SYS_SET_TONE) || (SysStatus == SYS_SET_CAMERA)|| (SysStatus == SYS_SET_UNLOCKKEY))
		if((SysStatus == SYS_SET_TONE) || (SysStatus == SYS_SET_CAMERA))
		{
			InputIdx = 1;
			InputRoom = key;
		}
		else if(SysStatus == SYS_SET_TALKVOLUM)
		{
			if((key > 0) && (key <= InputMax))
			{
				InputIdx = 1;
				InputRoom = key;
			}
		}
		else if((InputIdx < InputLen) && (InputMax >= (InputRoom*10)))
		{
			InputIdx++;
			InputRoom *= 10;
			if(InputMax >= (InputRoom + key))
			{
				InputRoom += key;
			}
			else
			{
				InputRoom = InputMax;
			}
		}
	}
	ShowSubPara();
}

void SysKey(u8 Key)
{
	IdleTime = 30;
	switch(SysStatus)
	{
		case SYS_IDLE:
			SysIdleKey(Key);
			break;
		case SYS_SETTING:
			SysSettingKey(Key);
			break;
		case SYS_SET_NAME:
			NameSetKey(Key);
			break;
		case SYS_EDIT_NAME:
			NameEditKey(Key);
			break;
		case SYS_DEL_NAME:
			NameDeleteKey(Key);
			break;
		case SYS_DEL_ALLNAME:
			NameDelAllKey(Key);
			break;
		case SYS_SET_PSW:
			PswSetKey(Key);
			break;
		case SYS_EDIT_PSW:
			PswEditKey(Key);
			break;
		case SYS_DEL_PSW:
			PswDeleteKey(Key);
			break;
		case SYS_DEL_ALLPSW:
			PswDelAllKey(Key);
			break;
		case SYS_INPUT_PSW:
			PswInputKey(Key);
			break;
		case SYS_INPUT_ROOM:
			SysInputRoom(Key);
			break;
		case SYS_INPUT_SYSPSW:
			InputSysPsw(Key);
			break;
		case SYS_EDIT_SYSPSW:
			SysPswEditKey(Key);
			break;
		case SYS_CALL:
		case SYS_TALK:
			SysTalkKey(Key);
			break;
//		case SYS_SET_TALKTIME:
//		case SYS_SET_RINGIN:
		case SYS_SET_RINGOUT:
		case SYS_SET_TONE:
		case SYS_SET_CAMERA:
		case SYS_SET_RELAY:
//		case SYS_SET_UNLOCKKEY:
		case SYS_SET_TALKVOLUM:
			SysSubSetKey(Key);
			break;
		case SYS_SET_CARD:
			CardSetKey(Key);
			break;
		case SYS_EDIT_CARD:
			CardEditKey(Key);
			break;
		case SYS_DEL_CARD:
			CardDeleteKey(Key);
			break;
		case SYS_DEL_ALLCARD:
			CardDelAllKey(Key);
			break;
	}
	if((IdleTime > 10) && ((SysStatus == SYS_INPUT_SYSPSW) || (SysStatus == SYS_INPUT_PSW) || (SysStatus == SYS_INPUT_ROOM) || (SysStatus == SYS_IDLE)))
	{
		IdleTime = 10;
	}
}

void SysCard(u8 * card)
{
	switch(SysStatus)
	{
		case SYS_EDIT_CARD:
		case SYS_DEL_CARD:
			CardEditCardIn(card);
			break;
		case SYS_DEL_ALLCARD:
		case SYS_SET_CARD:
			break;
		case SYS_IDLE:
		default:
			if(CheckCard(card) != 0xFFFF)
			{
				Unlock(true);
			}
			else
			{
				ShowCardErr();
			}
			if((SysStatus != SYS_TALK) && (SysStatus != SYS_CALL))
			{
				IdleTime = 3;
			}
			break;
		
	}
}
