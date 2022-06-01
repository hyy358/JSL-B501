#include "Driver.h"
#include "NameList.h"
#include "Eeprom.h"
#include "SysString.h"
#include "stdio.h"
#include "Lcd12864.h"
#include "SysStatus.h"
#include "Audio.h"

void SaveNameCount(void)
{
	writeBuf_at24c02(USER_PARA_ADDR+((u16)((u32)(&UsrPara.NameListCount)-(u32)(&UsrPara))), (u8 *)&UsrPara.NameListCount,sizeof(UsrPara.NameListCount));
}

void ClearNameList(void)
{
	UsrPara.NameListCount = 0;
	SaveNameCount();
}

void InitNameList(void)
{
	if(UsrPara.NameListCount > NAME_LIST_COUNT)
	{
		ClearNameList();
	}
}


void WriteNameItemByIdx(u16 idx,NameItemType *pItem)
{
	u16 addr = (u16)idx;
	addr *= sizeof(NameItemType);
	writeBuf_at24c02(NAMELIST_ADDR+addr, (u8 *)pItem,sizeof(NameItemType));
}

void ReadNameItemByIdx(u16 idx,NameItemType *pItem)
{
	u16 addr = (u16)idx;
	addr *= sizeof(NameItemType);
	readBuf_at24c02(NAMELIST_ADDR+addr, (u8 *)pItem,sizeof(NameItemType));
}

u16 GetNameItemByRoom(u16 room,NameItemType *pItem)
{
	u16 idx = 0xFFFF;
	idx = GetIdxByRoom(room,NAME_LIST);
	if(idx != 0xFFFF)
	{
		ReadNameItemByIdx(idx,pItem);
		return idx;
	}
	return 0xFFFF;
}
u16 AddNameItem(NameItemType *pItem)
{
	u16 idx = 0xFFFF;
	idx = GetIdxByRoom(pItem->Room,NAME_LIST);
	if(idx != 0xFFFF)
	{
		WriteNameItemByIdx(idx,pItem);
		return idx;
	}
	else if(UsrPara.NameListCount < NAME_LIST_COUNT)
	{
		WriteNameItemByIdx(UsrPara.NameListCount,pItem);
		UsrPara.NameListCount++;
		SaveNameCount();
		return UsrPara.NameListCount-1;
	}
	return 0xFFFF;
}
void DelNameItemByIdx(u16 idx)
{
	NameItemType tmpItem;
	if(idx < UsrPara.NameListCount)
	{
		if(idx == (UsrPara.NameListCount-1))
		{
			UsrPara.NameListCount--;
			SaveNameCount();
		}
		else
		{
			UsrPara.NameListCount--;
			SaveNameCount();
			ReadNameItemByIdx(UsrPara.NameListCount,&tmpItem);
			WriteNameItemByIdx(idx,&tmpItem);
		}
	}
}
void DelNameItemByRoom(u16 room)
{
	u16 idx = GetIdxByRoom(room,NAME_LIST);
	if(idx != -1)
	{
		DelNameItemByIdx(idx);
	}
}

NameItemType NameEditItem;
static u8 NameEditType = 0;
static u8 NameEditIdx = 0;
static bool NameEditSaved = false;
u16 EditTime = 0;
u8 EditChar = 0;
void ShowName(NameItemType *pItem,bool tel)
{
	u8 Disbuf[80];
	u8 tmp;
	LcdClear();
	if(pItem->Room > 0)
	{
	//	sprintf(Disbuf,"%s%u",StrTabRoom,pItem->Room);
	//	DisplayString(0,0,Disbuf);
		DisplayString(0,0,StrTabRoom);
		sprintf(Disbuf,"%u",pItem->Room);
		DisplayString(0,(16-HBstrlen(StrTabRoom)-strlen(Disbuf)),Disbuf);
		if((NameEditType == 0) && ((SysStatus == SYS_EDIT_NAME)||(SysStatus == SYS_DEL_NAME)))
		{
		#ifdef LANGUAGE_HB	
			SetFlash(0,(15-HBstrlen(StrTabRoom)));
		#else
			SetFlash(0,strlen(Disbuf));
		#endif
		}
	}
	else
	{
		DisplayString(0,0,StrTabRoom);
		if((NameEditType == 0) && ((SysStatus == SYS_EDIT_NAME)||(SysStatus == SYS_DEL_NAME)))
		{
		#ifdef LANGUAGE_HB	
			SetFlash(0,(15-HBstrlen(StrTabRoom)));
		#else
			SetFlash(0,strlen(StrTabRoom));
		#endif
		}
	}
	
	sprintf(Disbuf,"%s%s",StrTabName,pItem->Name);
	DisplayString(1,0,Disbuf);
	if((NameEditType == 1) && ((SysStatus == SYS_EDIT_NAME)||(SysStatus == SYS_DEL_NAME)))
	{
	#ifdef LANGUAGE_HB	
		tmp = HBstrlen(pItem->Name);
		tmp -= (EditTime==0?0:1);
		if(tmp <= 12)
		{
			SetFlash(1,(12-tmp));
		}
		else
		{
			SetFlash(2,(28-tmp));
		}
	#else
		SetFlash(1,strlen(Disbuf));
	#endif
	}
	if(tel)
	{
		if(pItem->Tel)
		{
		#ifdef LANGUAGE_HB
			DisplayString(3,0,StrTabTel);
			sprintf(Disbuf,"%u",pItem->Tel);
			DisplayString(3,(15-HBstrlen(StrTabTel)-strlen(Disbuf)),Disbuf);
			if((NameEditType == 2) && ((SysStatus == SYS_EDIT_NAME)||(SysStatus == SYS_DEL_NAME)))
			{
				SetFlash(3,(15-HBstrlen(StrTabTel)));
			}
		#else
			sprintf(Disbuf,"%s%u",StrTabTel,pItem->Tel);
			DisplayString(3,0,Disbuf);
			if((NameEditType == 2) && ((SysStatus == SYS_EDIT_NAME)||(SysStatus == SYS_DEL_NAME)))
			{
				SetFlash(3,strlen(Disbuf));
			}
		#endif
		}
		else
		{
			DisplayString(3,0,StrTabTel);
			if((NameEditType == 2) && ((SysStatus == SYS_EDIT_NAME)||(SysStatus == SYS_DEL_NAME)))
			{
			#ifdef LANGUAGE_HB	
				SetFlash(3,(15-HBstrlen(StrTabTel)));
			#else
				SetFlash(3,strlen(Disbuf));
			#endif
			}
		}
	}
}

void EditNameItemCon(void)
{
	ClearBuf((u8 *)&NameEditItem,sizeof(NameEditItem));
	NameEditType = 0;
	NameEditIdx = 0;
	SysStatus = SYS_EDIT_NAME;
	ShowName(&NameEditItem,true);
}
void NameEditChageType(void)
{
	u16 tmp;
	NameItemType tmpItem;
	switch(NameEditType)
	{
		case 0:
			NameEditType = 0;
			NameEditIdx = 0;
			tmp = NameEditItem.Room;
			while(tmp > 0)
			{
				NameEditIdx++;
				tmp /= 10;
			}
			break;
		case 1:
			tmp = NameEditItem.Room;
			if(GetNameItemByRoom(tmp,&tmpItem) != 0xFFFF)
			{
				HS_memcpy((u8*)&NameEditItem,(u8*)&tmpItem,sizeof(NameEditItem));
			}
			NameEditIdx = strlen(NameEditItem.Name);
			break;
		case 2:
			NameEditIdx = 0;
			tmp = NameEditItem.Tel;
			while(tmp > 0)
			{
				NameEditIdx++;
				tmp /= 10;
			}
			break;
		case 3:
			ShowConfirmSave();
			NameEditSaved = false;
			break;
	}
}

code u8 HBtab[27]={0x96,0x97,0x98,0x93,0x94,0x95,0x90,0x91,0x92,0xA0,0x9F,0xA1,0x9C,0x9E,0x9D,0x99,0x9B,0x9A,0xA8,0xA9,0xAA,0xA6,0xA5,0xA7,0xA2,0xA4,0xA3};
void NameEditKey(u8 key)
{
//	u16 tmp;
//	NameItemType tmpItem;
	static u8 OldKey = 0xFF;
	u8 HBtmp;
	
	if(key == KEY_SURE)
	{
		if(NameEditType == 3)
		{
			if(NameEditSaved)
			{
				ClearBuf((u8 *)&NameEditItem,sizeof(NameEditItem));
				NameEditIdx = 0;
			}
			else
			{
				if((NameEditItem.Room > 0)
				//	&& (strlen(NameEditItem.Name) > 0)
					&& (NameEditItem.Tel > 0))
				{
					if(AddNameItem(&NameEditItem) != 0xFFFF)
					{
						ShowSaveOK();
					}
					else
					{
						ShowErr();
					}
					ClearBuf((u8 *)&NameEditItem,sizeof(NameEditItem));
				}
				else
				{
					ShowErr();
				}
				NameEditSaved = true;
				return;
			}
		}
		NameEditType++;
		NameEditType %= 4;
		NameEditChageType();
		EditChar = 0;
		EditTime = 0;
		if(NameEditType == 3)
		{
			return;
		}
	}
	else if(key == KEY_DOWN)
	{
		if(NameEditType < 4)
		{
			NameEditType++;
		}
		else
		{
			NameEditType = 0;
		}
		NameEditChageType();
		EditChar = 0;
		EditTime = 0;
	}
	else if(key == KEY_UP)
	{
		if(NameEditType > 0)
		{
			NameEditType--;
		}
		else
		{
			NameEditType = 2;
		}
		NameEditChageType();
		EditChar = 0;
		EditTime = 0;
	}
	else if(key == KEY_DEL)
	{
		switch(NameEditType)
		{
			case 0:
				if(NameEditIdx > 0)
				{
					NameEditIdx--;
					NameEditItem.Room /= 10;
				}
				else
				{
					NameSetCon();
					return;
				}
				break;
			case 1:
				if((EditTime == 0) && (NameEditItem.Name[NameEditIdx] == 0))
				{
					if(NameEditIdx > 0)
					{
						NameEditItem.Name[NameEditIdx] = 0;
						NameEditIdx--;
						if(NameEditItem.Name[NameEditIdx] > 0x80)
						{
							NameEditItem.Name[NameEditIdx] = 0;
							NameEditIdx--;
						}
					}
					else
					{
						NameEditType = 0;
						NameEditChageType();
					}
				}
				NameEditItem.Name[NameEditIdx] = 0;
				NameEditItem.Name[NameEditIdx+1] = 0;
				break;
			case 2:
				if(NameEditIdx > 0)
				{
					NameEditIdx--;
					NameEditItem.Tel /= 10;
				}
				else
				{
					NameEditType--;
					NameEditChageType();
				}
				break;
			case 3:
				NameEditType = 0;
				NameEditChageType();
				break;
		}
		EditChar = 0;
		EditTime = 0;
	}
	else if(key == KEY_BELL)
	{
		NameSetCon();
		return;
	}
	else if((key == KEY_TIMEOUT) && (NameEditType == 1) && (NameEditItem.Name[NameEditIdx] != 0))
	{
		if(NameEditItem.Name[NameEditIdx] > 0x80)
		{
			NameEditIdx++;
		}
		NameEditIdx++;
		EditChar = 0;
		EditTime = 0;
	}
	if((key >= 0) && (key <= 9))
	{
		switch(NameEditType)
		{
			case 0:
				if(NameEditIdx < 4)
				{
					NameEditItem.Room *= 10;
					NameEditItem.Room += key;
					NameEditIdx++;
				}
				if(NameEditItem.Room == 0)
				{
					NameEditIdx = 0;
				}
				break;
			case 1:
				if((HBstrlen(NameEditItem.Name) >= NAME_LEN))
				{
					if((EditTime == 0) || (OldKey != key))
					{
						return;
					}
				}
				if((OldKey != key) && (NameEditItem.Name[NameEditIdx] != 0))
				{
					if(NameEditItem.Name[NameEditIdx] > 0x80)
					{
						NameEditIdx++;
					}
					NameEditIdx++;
					EditChar = 0;
				}
				
				if(key == 0)
				{
					NameEditItem.Name[NameEditIdx] = FhChar[EditChar];
				}
				else if(EditChar < 3)
				{
					NameEditItem.Name[NameEditIdx] = 0xD7;
					NameEditItem.Name[NameEditIdx+1] = HBtab[(key-1)*3+EditChar];//HBtmp+EditChar;
				}
				else if(EditChar == 3)
				{
					NameEditItem.Name[NameEditIdx+1] = 0;
					NameEditItem.Name[NameEditIdx] = key+'0';
				}
				else
				{
					NameEditItem.Name[NameEditIdx+1] = 0;
					HBtmp = EditChar - 4;
					if(key == 9)
					{
						if(HBtmp < 2)
							NameEditItem.Name[NameEditIdx] = HBtmp+'Y';
						else if(HBtmp == 2)
							NameEditItem.Name[NameEditIdx] = '.';
						else if(EditChar == 5)
							NameEditItem.Name[NameEditIdx] = ',';
						else
							NameEditItem.Name[NameEditIdx] = HBtmp-3+'y';
					}
					else
					{
						NameEditItem.Name[NameEditIdx] = (key-1)*3+HBtmp + (HBtmp>2?'a'-3:'A');
					}
				}
				EditChar++;
				EditChar %= 10;
				EditTime = 1000;
			
				break;
			case 2:
				if(NameEditIdx < 3)
				{
					NameEditItem.Tel *= 10;
					NameEditItem.Tel += key;
					NameEditIdx++;
				}
				if(NameEditItem.Tel == 0)
				{
					NameEditIdx = 0;
				}
				break;
		}
	}
	
	OldKey = key;
	ShowName(&NameEditItem,true);
}

void DeleteNameCon(void)
{
	ClearBuf((u8 *)&NameEditItem,sizeof(NameEditItem));
	NameEditType = 0;
	NameEditIdx = 0;
	SysStatus = SYS_DEL_NAME;
	ShowName(&NameEditItem,false);
}

void NameDeleteKey(u8 key)
{
	u16 tmp;
	NameItemType tmpItem;
	if(key == KEY_SURE)
	{
		if(NameEditType == 2)
		{
			if(NameEditSaved)
			{
				ClearBuf((u8 *)&NameEditItem,sizeof(NameEditItem));
				NameEditIdx = 0;
			}
			else
			{
				DelNameItemByRoom(NameEditItem.Room);
				NameEditSaved = true;
				return;
			}
		}
		NameEditType++;
		NameEditType %= 3;
		switch(NameEditType)
		{
			case 0:
				NameEditIdx = 0;
				tmp = NameEditItem.Room;
				while(tmp > 0)
				{
					NameEditIdx++;
					tmp /= 10;
				}
				break;
			case 1:
				if(GetNameItemByRoom(NameEditItem.Room,&tmpItem) != 0xFFFF)
				{
					HS_memcpy((u8*)&NameEditItem,(u8*)&tmpItem,sizeof(NameEditItem));
				}
				NameEditIdx = strlen(NameEditItem.Name);
				break;
			case 2:
				ShowConfirmDel();
				NameEditSaved = false;
				return;
		}
		EditChar = 0;
		EditTime = 0;
	}
	else if(key == KEY_DEL)
	{
		switch(NameEditType)
		{
			case 0:
				if(NameEditIdx > 0)
				{
					NameEditIdx--;
					NameEditItem.Room /= 10;
				}
				else
				{
					NameSetCon();
					return;
				}
				break;
			case 1:
				NameEditType = 0;
				NameEditIdx = 0;
				NameEditItem.Name[0] = 0;
				tmp = NameEditItem.Room;
				while(tmp > 0)
				{
					NameEditIdx++;
					tmp /= 10;
				}
				break;
			case 2:
				NameEditType = 0;
				NameEditItem.Name[0] = 0;
				tmp = NameEditItem.Room;
				NameEditIdx = 0;
				while(tmp > 0)
				{
					NameEditIdx++;
					tmp /= 10;
				}
				break;
		}
		EditChar = 0;
		EditTime = 0;
	}
	else if(key == KEY_BELL)
	{
		NameSetCon();
		return;
	}
	if((key >= 0) && (key <= 9) && (NameEditType == 0))
	{
		if(NameEditIdx < 4)
		{
			NameEditItem.Room *= 10;
			NameEditItem.Room += key;
			NameEditIdx++;
		}
		if(NameEditItem.Room == 0)
		{
			NameEditIdx = 0;
		}
	}
	ShowName(&NameEditItem,false);
}

void DeleteAllNameCon(void)
{
	SysStatus = SYS_DEL_ALLNAME;
	LcdClear();
	DisplayString(1,0,StrTabDelAllName);
}
void NameDelAllKey(u8 key)
{
	switch(key)
	{
		case KEY_SURE:
			ClearNameList();
			ShowDeleteOK();
			break;
		case KEY_BELL:
		case KEY_DEL:
			NameSetCon();
			break;
	}
}

void NameSetCon(void)
{
	u8 Disbuf[10];
	SysStatus = SYS_SET_NAME;
	LcdClear();
	DisplayString(0,0,StrTabNameSet1);
	DisplayString(1,0,StrTabNameSet2);
	DisplayString(2,0,StrTabNameSet3);
	DisplayString(3,0,StrTabListCount);
	sprintf(Disbuf,"%u",UsrPara.NameListCount);
	DisplayString(3,15-HBstrlen(StrTabListCount)-strlen(Disbuf),Disbuf);
}
void NameSetKey(u8 key)
{
	switch(key)
	{
		case 1:
			EditNameItemCon();
			break;
		case 2:
			DeleteNameCon();
			break;
		case 3:
			DeleteAllNameCon();
			break;
		case KEY_DEL:
		case KEY_BELL:
			SysSettingCon();
			break;
	}
}

void ShowNameByIdx(u16 idx)
{
	NameItemType tmpItem;
	if(idx < UsrPara.NameListCount)
	{
		ReadNameItemByIdx(idx,&tmpItem);
		ShowName(&tmpItem,false);
	}
}

//bool CallByIdx(u16 idx,u8 telIdx)
//{
//	u8 Disbuf[20];
//	NameItemType tmpItem;
//	ReadNameItemByIdx(idx,&tmpItem);
//	if(strlen(tmpItem.Tel[telIdx]) > 0)
//	{
//		if(GSM_Call(tmpItem.Tel[telIdx]))
//		{
//			sprintf(Disbuf,"%s%s",StrTabCall,tmpItem.Tel[telIdx]);
//			DisplayString(1,0,Disbuf);
//			return true;
//		}
//		return false;
//	}
//	return false;
//}

//bool CallByRoom(u16 Room,u8 telIdx)
//{
//	u8 Disbuf[20];
//	NameItemType tmpItem;
//	if(GetNameItemByRoom(Room,&tmpItem) != 0xFFFF)
//	{
//		if(strlen(tmpItem.Tel[telIdx]) > 0)
//		{
//			if(GSM_Call(tmpItem.Tel[telIdx]))
//			{
//				sprintf(Disbuf,"%s%s",StrTabCall,tmpItem.Tel[telIdx]);
//				DisplayString(1,0,Disbuf);
//				return true;
//			}
//		}
//	}
//	return false;
//}

bool CallByNameItem(NameItemType *NameItem)
{
	u8 Disbuf[20];
	if(NameItem->Tel > 0)
	{
		//GSM_Call(NameItem->Tel))

			LcdClear();
			RelayOpen(NameItem->Tel,SysPara.RingOutTs);
		#ifdef PWM_BEEP
			MP3_StartPlay(MP3_Ring);
		#endif
		#ifdef PWM_RING
			StartRing(SysPara.RingOutTs);
		#endif
			//sprintf(Disbuf,"%s%u",StrTabCall,NameItem->Tel);
			if(strlen(NameItem->Name) == 0)
			{
				//sprintf(Disbuf,"%s%u",StrTabCall,NameItem->Room);
				DisplayString(1,0,StrTabCall);
				if(NameItem->Room > 0)
				{
					sprintf(Disbuf,"%u",NameItem->Room);
					DisplayString(1,(15-HBstrlen(StrTabCall)-strlen(Disbuf)),Disbuf);
				}
			}
			else
			{
				sprintf(Disbuf,"%s%s",StrTabCall,NameItem->Name);
				DisplayString(1,0,Disbuf);
			}
			
		return true;
	}
	return false;
}
#include "UART.h"
void RevNamelist(u8 *buf)
{
	int i;
	u16 cnt;
	u16 j;
	NameItemType tmpItem;
	u8  *p = buf;
	
	if(SysStatus != SYS_SETTING)
	{
		printf("Download Namelist Err:( not in setting )!\r\n");
		while(COM1.B_TX_busy);
		return;
	}
	IdleTime = 15;
	i = lookforstr(p,"C:");
	if(i != -1)
	{
		p += (i+2);
		sscanf(p,"%u",&cnt);
	}
	printf("RevNamelist(Cnt:%u)\r\n",cnt);
	for(i=0;i<cnt;i++)
	{
		j = lookforstr(p,"R:");
		if(j != -1)
		{
			p += j+2;
			
			ClearBuf((u8 *)&tmpItem,sizeof(tmpItem));
			sscanf(p,"%u;",&tmpItem.Room);

			j = lookforstr(p,"N:");
			if(j != -1)
			{
				p += j+2;
				//sscanf(p,"%s;",&tmpItem.Name);
				j = 0;
				while((*p != ';') && (j<(NAME_LEN*2+2)))
				{
					tmpItem.Name[j] = *p++;
					j++;
				}
				tmpItem.Name[j]  = 0;
			}
			
			j = lookforstr(p,"RY:");
			if(j != -1)
			{
				p += j+3;
				//sscanf(p,"%s;",&tmpItem.Name);
				j = 0;
				tmpItem.Tel = 0;
				while((*p != ';') && (j<TEL_LEN))
				{
					tmpItem.Tel *= 10;
					tmpItem.Tel += (*p -'0');
					j++;
					p++;
				}
			}
			
			
			j = AddNameItem(&tmpItem);
			printf("Idx:%d-(%d); R:%u; N:%s; RY:%u;\r\n",i+1,j+1,tmpItem.Room,tmpItem.Name,tmpItem.Tel);
			while(COM1.B_TX_busy);
		}
		
		if(j == 0xFFFF)
		{
			break;
		}
	}
	if(i == cnt)
	{
		printf("Download Namelist OK! #(%u)#\r\n",UsrPara.NameListCount);
	}
	else
	{
		printf("Download Namelist Err:(%u#%u)!\r\n",i,j);
	}
	while(COM1.B_TX_busy);
}
