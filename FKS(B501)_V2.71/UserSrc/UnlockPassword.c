#include "UnlockPassword.h"
#include "Driver.h"
#include "Eeprom.h"
#include "SysString.h"
#include "stdio.h"
#include "Lcd12864.h"
#include "SysStatus.h"
#include "NameList.h"
#include "Audio.h"

void SavePswCount(void)
{
	writeBuf_at24c02(USER_PARA_ADDR+((u16)((u32)(&UsrPara.PswCount)-(u32)(&UsrPara))), (u8 *)&UsrPara.PswCount,sizeof(UsrPara.PswCount));
}

void ClearPsw(void)
{
	UsrPara.PswCount = 0;
	SavePswCount();
}

void InitPsw(void)
{
	if(UsrPara.PswCount > PSW_COUNT)
	{
		ClearPsw();
	}
}

void WritePswByIdx(u16 idx,PswItemType *pItem)
{
	u16 addr = (u16)idx;
	addr *= sizeof(PswItemType);
	writeBuf_at24c02(USER_PSW_ADDR+addr, (u8 *)pItem,sizeof(PswItemType));
}

void ReadPswByIdx(u16 idx,PswItemType *pItem)
{
	u16 addr = (u16)idx;
	addr *= sizeof(PswItemType);
	readBuf_at24c02(USER_PSW_ADDR+addr, (u8 *)pItem,sizeof(PswItemType));
}

u16 GetPswByRoom(u16 room,PswItemType *pItem)
{
	u16 idx = 0xFFFF;
	idx = GetIdxByRoom(room,UNLOCK_PSW);
	if(idx != 0xFFFF)
	{
		ReadPswByIdx(idx,pItem);
		return idx;
	}
	return 0xFFFF;
}

bool CheckPsw(u16 psw)
{
	u16 i = 0;
	PswItemType pItem;
	printf("CheckPsw:%u\r\n",psw);
	if(UsrPara.PswCount == 0)
	{
		return false;
	}
	for(i=0;i<UsrPara.PswCount;i++)
	{
		ReadPswByIdx(i,&pItem);
		if(pItem.Psw == psw)
		{
			return true;
		}
	}
	return false;
}

u16 AddPswItem(PswItemType *pItem)
{
	u16 idx = -1;
	idx = GetIdxByRoom(pItem->Room,UNLOCK_PSW);
	if(idx != -1)
	{
		WritePswByIdx(idx,pItem);
		return idx;
	}
	else if(UsrPara.PswCount < PSW_COUNT)
	{
		WritePswByIdx(UsrPara.PswCount,pItem);
		UsrPara.PswCount++;
		SavePswCount();
		return UsrPara.PswCount-1;
	}
	return 0xFFFF;
}

void DelPswByIdx(u16 idx)
{
	PswItemType tmpItem;
	if(idx < UsrPara.PswCount)
	{
		if(idx == (UsrPara.PswCount-1))
		{
			UsrPara.PswCount--;
			SavePswCount();
		}
		else
		{
			UsrPara.PswCount--;
			SavePswCount();
			ReadPswByIdx(UsrPara.PswCount,&tmpItem);
			WritePswByIdx(idx,&tmpItem);
		}
	}
}

void DelPswByRoom(u16 room)
{
	u16 idx = GetIdxByRoom(room,UNLOCK_PSW);
	while(idx != 0xFFFF)
	{
		DelPswByIdx(idx);
		idx = GetIdxByRoom(room,UNLOCK_PSW);
	}
}

PswEditType PswEditItem;
static u8 PswEditMidx = 0;
static u8 PswEditIdx = 0;

void ShowPsw(PswEditType *pItem,bool edit)
{
	u8 Disbuf[20];
	u8 i;
	u32 tmp;
	LcdClear();
	//if(SysStatus != SYS_EDIT_SYSPSW)
	if(SysStatus == SYS_EDIT_SYSPSW)
	{
		DisplayString(1,0,StrTabSysPsw);
	}
	else if((SysStatus != SYS_EDIT_SYSPSW) && (SysStatus != SYS_INPUT_PSW))
	{
		if(pItem->Room > 0)
		{
			//sprintf(Disbuf,"%s%u",StrTabNum,pItem->Room);
			//DisplayString(1,0,Disbuf);
			DisplayString(1,0,StrTabNum);
			sprintf(Disbuf,"%u",pItem->Room);
			DisplayString(1,(16-HBstrlen(StrTabNum)-strlen(Disbuf)),Disbuf);
		}
		else
		{
			DisplayString(1,0,StrTabNum);
			//sprintf(Disbuf,"%s",StrTabNum);
		}
		//DisplayString(1,0,Disbuf);
		if(PswEditMidx == 0)
		{
		#ifdef LANGUAGE_HB	
			//SetFlash(1,(10-PswEditIdx));
			SetFlash(1,15-HBstrlen(StrTabNum));
		#else
			SetFlash(1,strlen(Disbuf));
		#endif
		}
	}
	if(edit)
	{
		if(pItem->Psw1 > 0)
		{
			//sprintf(Disbuf,"%s%lu",StrTabPsw1,pItem->Psw1);
			sprintf(Disbuf,"%s%",StrTabPsw1);
			tmp = PswEditItem.Psw1;
			for(i=0;(tmp > 0);i++)
			{
				Disbuf[strlen(StrTabPsw1)+i] = '*';
				tmp /= 10;
			}
			Disbuf[strlen(StrTabPsw1)+i] = 0;
			DisplayString(2,0,Disbuf);
			if(PswEditMidx == ((SysStatus != SYS_EDIT_SYSPSW)?1:0))
			{
			#ifdef LANGUAGE_HB	
				SetFlash(2,(9-PswEditIdx));
			#else
				SetFlash(2,strlen(Disbuf));
			#endif
			}
		}
		else
		{
			DisplayString(2,0,StrTabPsw1);
			if(PswEditMidx == ((SysStatus != SYS_EDIT_SYSPSW)?1:0))
			{
			#ifdef LANGUAGE_HB	
				SetFlash(2,9);
			#else
				SetFlash(2,strlen(StrTabPsw1));
			#endif
			}
		}
		if(SysStatus != SYS_INPUT_PSW)
		{
			if(pItem->Psw2 > 0)
			{
				//sprintf(Disbuf,"%s%lu",StrTabPsw2,pItem->Psw2);
				sprintf(Disbuf,"%s%",StrTabPsw2);
				tmp = PswEditItem.Psw2;
				for(i=0;(tmp > 0);i++)
				{
					Disbuf[strlen(StrTabPsw2)+i] = '*';
					tmp /= 10;
				}
				Disbuf[strlen(StrTabPsw2)+i] = 0;
				DisplayString(3,0,Disbuf);
				if(PswEditMidx == ((SysStatus != SYS_EDIT_SYSPSW)?2:1))
				{
				#ifdef LANGUAGE_HB	
					SetFlash(3,(11-PswEditIdx));
				#else
					SetFlash(3,strlen(Disbuf));
				#endif
				}
			}
			else
			{
				DisplayString(3,0,StrTabPsw2);
				if(PswEditMidx == ((SysStatus != SYS_EDIT_SYSPSW)?2:1))
				{
				#ifdef LANGUAGE_HB	
					SetFlash(3,11);
				#else
					SetFlash(3,strlen(StrTabPsw2));
				#endif
				}
			}
		}
	}
}

void EditPswCon(void)
{
	ClearBuf((u8 *)&PswEditItem,sizeof(PswEditType));
	PswEditMidx = 0;
	PswEditIdx = 0;
	SysStatus = SYS_EDIT_PSW;
	ShowPsw(&PswEditItem,true);
}

void PswEditKey(u8 key)
{
	u16 tmp;
	PswItemType tmpItem;
	static bool Saved = false;
	if(key == KEY_SURE)
	{
		if(PswEditMidx == 3)
		{
			if(Saved)
			{
				ClearBuf((u8 *)&PswEditItem,sizeof(PswEditItem));
				PswEditIdx = 0;
			}
			else
			{
				if((PswEditItem.Room > 0)
					&& (PswEditItem.Psw1 != 0)
					&& (PswEditItem.Psw1 == PswEditItem.Psw2))
				{
					tmpItem.Room = PswEditItem.Room;
					tmpItem.Psw = PswEditItem.Psw1;
					if(AddPswItem(&tmpItem) != 0xFFFF)
					{
						ShowSaveOK();
					}
					else
					{
						ShowErr();
					}
					ClearBuf((u8 *)&PswEditItem,sizeof(PswEditItem));
					Saved = true;
					PswEditItem.Psw1 = 0;
					PswEditItem.Psw2 = 0;
				}
				else
				{
					ShowErr();
				}
				return;
			}
		}
		PswEditMidx++;
		PswEditMidx %= 4;
		PswEditIdx = 0;
		switch(PswEditMidx)
		{
			case 0:
				PswEditMidx = 0;
				tmp = PswEditItem.Room;
				while(tmp > 0)
				{
					PswEditIdx++;
					tmp /= 10;
				}
				break;
			case 1:
				PswEditItem.Psw1 = 0;
				PswEditIdx = 0;
//				tmp = PswEditItem.Psw1;
//				while(tmp > 0)
//				{
//					PswEditIdx++;
//					tmp /= 10;
//				}
				break;
			case 2:
				PswEditItem.Psw2 = 0;
				PswEditIdx = 0;
//				tmp = PswEditItem.Psw2;
//				while(tmp > 0)
//				{
//					PswEditIdx++;
//					tmp /= 10;
//				}
				break;
			case 3:
				ShowConfirmSave();
				Saved = false;
				return;
		}
	}
	else if(key == KEY_DEL)
	{
		switch(PswEditMidx)
		{
			case 0:
				if(PswEditIdx > 0)
				{
					PswEditIdx--;
					PswEditItem.Room /= 10;
				}
				else
				{
					PswSetCon();
					return;
				}
				break;
			case 1:
				if(PswEditIdx > 0)
				{
					PswEditIdx--;
					PswEditItem.Psw1 /= 10;
				}
				else
				{
					PswEditMidx--;
					PswEditIdx = 0;
					tmp = PswEditItem.Room;
					while(tmp > 0)
					{
						PswEditIdx++;
						tmp /= 10;
					}
				}
				break;
			case 2:
				if(PswEditIdx > 0)
				{
					PswEditIdx--;
					PswEditItem.Psw2 /= 10;
				}
				else
				{
					PswEditMidx--;
					PswEditIdx = 0;
					tmp = PswEditItem.Psw1;
					while(tmp > 0)
					{
						PswEditIdx++;
						tmp /= 10;
					}
				}
				break;
			case 3:
				PswEditMidx = 0;
				PswEditIdx = 0;
				tmp = PswEditItem.Room;
				while(tmp > 0)
				{
					PswEditIdx++;
					tmp /= 10;
				}
				break;
		}
	}
	else if(key == KEY_BELL)
	{
		PswSetCon();
		return;
	}

	if((key >= 0) && (key <= 9))
	{
		switch(PswEditMidx)
		{
			case 0:
				if(PswEditIdx < 4)
				{
					PswEditItem.Room *= 10;
					PswEditItem.Room += key;
					PswEditIdx++;
					if(PswEditItem.Room > PSW_COUNT)
					{
						PswEditItem.Room = key;
						PswEditIdx = 0;
					}
				}
				
				if(PswEditItem.Room == 0)
				{
					PswEditIdx = 0;
				}
				break;
			case 1:
				if(PswEditIdx < 4)
				{
					PswEditItem.Psw1 *= 10;
					PswEditItem.Psw1 += key;
					PswEditIdx++;
				}
				if(PswEditItem.Psw1 == 0)
				{
					PswEditIdx = 0;
				}
				break;
			case 2:
				if(PswEditIdx < 4)
				{
					PswEditItem.Psw2 *= 10;
					PswEditItem.Psw2 += key;
					PswEditIdx++;
				}
				if(PswEditItem.Psw2 == 0)
				{
					PswEditIdx = 0;
				}
				break;
		}
	}
	ShowPsw(&PswEditItem,true);
}

void DeletePswCon(void)
{
	ClearBuf((u8 *)&PswEditItem,sizeof(PswEditType));
	PswEditMidx = 0;
	PswEditIdx = 0;
	SysStatus = SYS_DEL_PSW;
	ShowPsw(&PswEditItem,false);
}

void PswDeleteKey(u8 key)
{
	u16 tmp;
	static bool Saved = false;
	if(key == KEY_SURE)
	{
		if(PswEditMidx == 1)
		{
			if(Saved)
			{
				ClearBuf((u8 *)&PswEditItem,sizeof(PswEditItem));
				PswEditIdx = 0;
			}
			else
			{
				if(PswEditItem.Room > 0)
				{
					DelPswByRoom(PswEditItem.Room);
					ClearBuf((u8 *)&PswEditItem,sizeof(PswEditItem));
					Saved = true;
					ShowDeleteOK();
				}
				return;
			}
		}
		PswEditMidx++;
		PswEditMidx %= 2;
		PswEditIdx = 0;
		switch(PswEditMidx)
		{
			case 0:
				PswEditIdx = 0;
				tmp = PswEditItem.Room;
				while(tmp > 0)
				{
					PswEditIdx++;
					tmp /= 10;
				}
				break;
			case 1:
				ShowConfirmDel();
				Saved = false;
				return;
				break;
		}
	}
	else if(key == KEY_DEL)
	{
		switch(PswEditMidx)
		{
			case 0:
				if(PswEditIdx > 0)
				{
					PswEditIdx--;
					PswEditItem.Room /= 10;
				}
				else
				{
					PswSetCon();
					return;
				}
				break;
			case 1:
				PswEditMidx = 0;
				PswEditIdx = 0;
				tmp = PswEditItem.Room;
				while(tmp > 0)
				{
					PswEditIdx++;
					tmp /= 10;
				}
				break;
		}
	}
	else if(key == KEY_BELL)
	{
		PswSetCon();
		return;
	}

	if((key >= 0) && (key <= 9))
	{
		switch(PswEditMidx)
		{
			case 0:
				if(PswEditIdx < 4)
				{
					PswEditItem.Room *= 10;
					PswEditItem.Room += key;
					PswEditIdx++;
					if(PswEditItem.Room > PSW_COUNT)
					{
						PswEditItem.Room = key;
						PswEditIdx = 0;
					}
				}
				if(PswEditItem.Room == 0)
				{
					PswEditIdx = 0;
				}
				break;
		}
	}
	ShowPsw(&PswEditItem,false);
}

void DeleteAllPswCon(void)
{
	SysStatus = SYS_DEL_ALLPSW;
	LcdClear();
	DisplayString(1,0,StrTabDelAllPsw);
}
void PswDelAllKey(u8 key)
{
	switch(key)
	{
		case KEY_SURE:
			ClearPsw();
			ShowDeleteOK();
			break;
		case KEY_BELL:
		case KEY_DEL:
			PswSetCon();
			break;
	}
}

void PswSetCon(void)
{
	u8 Disbuf[10];
	
	SysStatus = SYS_SET_PSW;
	
	LcdClear();
	DisplayString(0,0,StrTabPswSet1);
	DisplayString(1,0,StrTabPswSet2);
	DisplayString(2,0,StrTabPswSet3);
	DisplayString(3,0,StrTabListCount);
	sprintf(Disbuf,"%u",UsrPara.PswCount);
	DisplayString(3,15-HBstrlen(StrTabListCount)-strlen(Disbuf),Disbuf);
}

void PswSetKey(u8 key)
{
	switch(key)
	{
		case 1:
			EditPswCon();
			break;
		case 2:
			DeletePswCon();
			break;
		case 3:
			DeleteAllPswCon();
			break;
		case KEY_DEL:
		case KEY_BELL:
			SysSettingCon();
			break;
	}
}

void InputPswCon(void)
{
	ClearBuf((u8 *)&PswEditItem,sizeof(PswEditType));
	PswEditMidx = 1;
	PswEditIdx = 0;
	SysStatus = SYS_INPUT_PSW;
	ShowPsw(&PswEditItem,true);
	MP3_StartPlay(MP3_InputPsw);
}

void PswInputKey(u8 key)
{
//	u16 tmp;
//	PswItemType tmpItem;
/*	if(key == KEY_SURE)
	{
		if(PswEditMidx == 1)
		{
			if((PswEditItem.Room > 0)
				&& (PswEditIdx == 4))
			{
				if(GetPswByRoom(PswEditItem.Room,&tmpItem) != 0xFFFF)
				{
					if(tmpItem.Psw == PswEditItem.Psw1)
					{
						Unlock();
						IdleTime = 4;
						PswEditItem.Psw1 = 0;
						PswEditItem.Room = 0;
						PswEditIdx = 0;
						PswEditMidx = 0;
						return;
					}
					else
					{
						PswEditItem.Psw1 = 0;
						PswEditIdx = 0;
						ShowErr();
						MP3_StartPlay(MP3_PwdErr);
					}
				}
//				else if(PswEditItem.Room > 0)
//				{
//					PswEditItem.Psw1 = 0;
//					PswEditItem.Room = 0;
//					PswEditIdx = 0;
//					MP3_StartPlay(MP3_RoomErr);
//				}
			}
			else if(PswEditItem.Room > 0)
			{
				PswEditItem.Psw1 = 0;
				PswEditIdx = 0;
				ShowErr();
				MP3_StartPlay(MP3_PwdErr);
			}
			
			IdleTime = 5;
			return;
		}
		PswEditMidx++;
		PswEditMidx %= 2;
		PswEditIdx = 0;
		switch(PswEditMidx)
		{
			case 0:
				PswEditMidx = 0;
				tmp = PswEditItem.Room;
				while(tmp > 0)
				{
					PswEditIdx++;
					tmp /= 10;
				}
				break;
			case 1:
				if(GetPswByRoom(PswEditItem.Room,&tmpItem) == 0xFFFF)
				{
					MP3_StartPlay(MP3_RoomErr);
					ShowErr();
					IdleTime = 5;
					PswEditItem.Room = 0;
					PswEditIdx = 0;
					return;
				}
				PswEditItem.Psw1 = 0;
				MP3_StartPlay(MP3_InputPsw);
				break;
		}
	}
	else */
	if(key == KEY_DEL)
	{
		switch(PswEditMidx)
		{
//			case 0:
//				if(PswEditIdx > 0)
//				{
//					PswEditIdx--;
//					PswEditItem.Room /= 10;
//				}
//				else
//				{
//				#ifdef PWM_AUDIO
//					MP3_Stop();
//				#endif
//					SysIdleCon();
//					return;
//				}
//				break;
			case 1:
				if(PswEditIdx > 0)
				{
					PswEditIdx--;
					PswEditItem.Psw1 /= 10;
				}
				else
				{
					SysIdleCon();
				#ifdef PWM_AUDIO
					MP3_Stop();
				#endif
					return;
				}
				break;
		}
	}
	else if(key == KEY_BELL)
	{
		SysIdleCon();
		#ifdef PWM_AUDIO
			MP3_Stop();
		#endif
		return;
	}

	if((key >= 0) && (key <= 9))
	{
		switch(PswEditMidx)
		{
//			case 0:
//				if(PswEditIdx < 4)
//				{
//					PswEditItem.Room *= 10;
//					PswEditItem.Room += key;
//					PswEditIdx++;
//				}
//				if(PswEditItem.Room == 0)
//				{
//					PswEditIdx = 0;
//				}
//				break;
			case 1:
				if(PswEditIdx < 4)
				{
					PswEditItem.Psw1 *= 10;
					PswEditItem.Psw1 += key;
					PswEditIdx++;
				}
				if(PswEditItem.Psw1 == 0)
				{
					PswEditIdx = 0;
				}
				
//				if((PswEditItem.Room > 0)
//					&& (PswEditIdx == 4))
				if(PswEditIdx == 4)
				{
					//if(GetPswByRoom(PswEditItem.Room,&tmpItem) != 0xFFFF)
					{
						//if(tmpItem.Psw == PswEditItem.Psw1)
						if(CheckPsw(PswEditItem.Psw1))
						{
							Unlock(true);
							IdleTime = 3;
							PswEditItem.Psw1 = 0;
							PswEditItem.Room = 0;
							PswEditIdx = 0;
							PswEditMidx = 1;
							return;
						}
						else
						{
							PswEditItem.Psw1 = 0;
							PswEditIdx = 0;
							PswEditMidx = 1;
							ShowErr();
							MP3_StartPlay(MP3_PwdErr);
							IdleTime = 5;
						}
					}
//					else
//					{
//						PswEditItem.Psw1 = 0;
//						PswEditItem.Room = 0;
//						PswEditIdx = 0;
//						PswEditMidx = 0;
//						MP3_StartPlay(MP3_RoomErr);
//						IdleTime = 5;
//					}
					
					return;
				}
				break;
		}
	}
	ShowPsw(&PswEditItem,true);
}


void EditSysPswCon(void)
{
	ClearBuf((u8 *)&PswEditItem,sizeof(PswEditType));
	PswEditMidx = 0;
	PswEditIdx = 0;
	SysStatus = SYS_EDIT_SYSPSW;
	ShowPsw(&PswEditItem,true);
}
void SaveSysPsw(void)
{
	writeBuf_at24c02(SYS_PARA_ADDR+((u16)((u32)(&SysPara.SysPsw)-(u32)(&SysPara))), (u8 *)&SysPara.SysPsw,sizeof(SysPara.SysPsw));
}

void SysPswEditKey(u8 key)
{
	u16 tmp;
	static bool Saved = false;
	if(key == KEY_SURE)
	{
		if(PswEditMidx == 2)
		{
			if(Saved)
			{
				ClearBuf((u8 *)&PswEditItem,sizeof(PswEditItem));
				PswEditIdx = 0;
			}
			else
			{
				if((PswEditItem.Psw1 != 0)
					&& (PswEditItem.Psw1 == PswEditItem.Psw2))
				{
					SysPara.SysPsw= PswEditItem.Psw1;
					SaveSysPsw();
					ShowSaveOK();
					ClearBuf((u8 *)&PswEditItem,sizeof(PswEditItem));
					Saved = true;
					PswEditItem.Psw1 = 0;
					PswEditItem.Psw2 = 0;
				}
				else
				{
					PswEditItem.Psw1 = 0;
					PswEditItem.Psw2 = 0;
					ShowErr();
				}
				return;
			}
		}
		if((PswEditMidx == 1) && (PswEditIdx != 6))
		{
			return;
		}
		PswEditMidx++;
		PswEditMidx %= 3;
		PswEditIdx = 0;
		switch(PswEditMidx)
		{
			case 0:
				PswEditItem.Psw1 = 0;
				PswEditIdx = 0;
				break;
			case 1:
				PswEditItem.Psw2 = 0;
				PswEditIdx = 0;
				break;
			case 2:
				ShowConfirmSave();
				Saved = false;
				return;
		}
	}
	else if(key == KEY_DEL)
	{
		switch(PswEditMidx)
		{
			case 0:
				if(PswEditIdx > 0)
				{
					PswEditIdx--;
					PswEditItem.Psw1 /= 10;
				}
				else
				{
					SysSettingCon();
					return;
				}
				break;
			case 1:
				if(PswEditIdx > 0)
				{
					PswEditIdx--;
					PswEditItem.Psw2 /= 10;
				}
				else
				{
					PswEditMidx--;
					PswEditIdx = 0;
					tmp = PswEditItem.Psw1;
					while(tmp > 0)
					{
						PswEditIdx++;
						tmp /= 10;
					}
				}
				break;
			case 2:
				PswEditMidx = 0;
				PswEditIdx = 0;
				break;
		}
	}
	else if(key == KEY_BELL)
	{
		SysSettingCon();
		return;
	}

	if((key >= 0) && (key <= 9))
	{
		switch(PswEditMidx)
		{
			case 0:
				if(PswEditIdx < 6)
				{
					PswEditItem.Psw1 *= 10;
					PswEditItem.Psw1 += key;
					PswEditIdx++;
				}
				if(PswEditItem.Psw1 == 0)
				{
					PswEditIdx = 0;
				}
				break;
			case 1:
				if(PswEditIdx < 6)
				{
					PswEditItem.Psw2 *= 10;
					PswEditItem.Psw2 += key;
					PswEditIdx++;
				}
				if(PswEditItem.Psw2 == 0)
				{
					PswEditIdx = 0;
				}
				break;
		}
	}
	ShowPsw(&PswEditItem,true);
}

