#include "Driver.h"
#include "NameList.h"
#include "Eeprom.h"
#include "SysString.h"
#include "stdio.h"
#include "Lcd12864.h"
#include "SysStatus.h"
#include "GSM.h"

void SaveNameCount(void)
{
	writeBuf_at24c02(USER_PARA_ADDR+((uint16_t)((uint32_t)(&UsrPara.NameListCount)-(uint32_t)(&UsrPara))), (uint8_t *)&UsrPara.NameListCount,sizeof(UsrPara.NameListCount));
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


void WriteNameItemByIdx(uint16_t idx,NameItemType *pItem)
{
	uint16_t addr = idx;
	addr *= sizeof(NameItemType);
	writeBuf_at24c02(NAMELIST_ADDR+addr, (uint8_t *)pItem,sizeof(NameItemType));
//	printf("Write:R:%u; N:%s; T1:%s; T2:%s; T3:%s;\r\n",pItem->Room,pItem->Name,pItem->Tel[0],pItem->Tel[1],pItem->Tel[2]);
//	ReadNameItemByIdx(idx,pItem);
}

void ReadNameItemByIdx(uint16_t idx,NameItemType *pItem)
{
	uint16_t addr = (uint16_t)idx;
	addr *= sizeof(NameItemType);
	readBuf_at24c02(NAMELIST_ADDR+addr, (uint8_t *)pItem,sizeof(NameItemType));
//	printf("Read:R:%u; N:%s; T1:%s; T2:%s; T3:%s;\r\n",pItem->Room,pItem->Name,pItem->Tel[0],pItem->Tel[1],pItem->Tel[2]);
}

uint16_t GetNameItemByRoom(uint16_t room,NameItemType *pItem)
{
	uint16_t idx = 0xFFFF;
	idx = GetIdxByRoom(room,NAME_LIST);
	if(idx != 0xFFFF)
	{
		ReadNameItemByIdx(idx,pItem);
		return idx;
	}
	return 0xFFFF;
}
uint16_t AddNameItem(NameItemType *pItem)
{
	uint16_t idx = 0xFFFF;
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
void DelNameItemByIdx(uint16_t idx)
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
void DelNameItemByRoom(uint16_t room)
{
	uint16_t idx = GetIdxByRoom(room,NAME_LIST);
	if(idx != 0xFFFF)
	{
		DelNameItemByIdx(idx);
	}
}
NameItemType NameEditItem;
static uint8_t NameEditType = 0;
static uint8_t NameEditIdx = 0;
static bool NameEditSaved = false;
uint16_t EditTime = 0;
uint8_t EditChar = 0;
void ShowName(NameItemType *pItem,bool tel)
{
	uint8_t Disbuf[80];
	uint8_t i;
	LcdClear();

	if(tel)
	{
		for(i=0;i<3;i++)
		{
			sprintf(Disbuf,"%s%bd:",StrTabTel,i+1);//,pItem->Tel[i]);
			DisplayString(1+i,0,Disbuf);
			DisplayString(1+i,(16-HBstrlen(Disbuf)-strlen(pItem->Tel[i])),pItem->Tel[i]);
			
			if((NameEditType == (2+i)) && ((SysStatus == SYS_EDIT_NAME)||(SysStatus == SYS_DEL_NAME)))
			{
			#ifdef LANGUAGE_HB	
				SetFlash(1+i,15-HBstrlen(Disbuf));//(11-NameEditIdx));
			#else
				SetFlash(1+i,strlen(Disbuf));
			#endif
			}
		}
	}
	else
	{
		if(pItem->Room > 0)
		{
			//sprintf(Disbuf,"%s%u",StrTabRoom,pItem->Room);
			DisplayString(1,0,StrTabRoom);
			sprintf(Disbuf,"%u",pItem->Room);
			DisplayString(1,(16-HBstrlen(StrTabRoom)-strlen(Disbuf)),Disbuf);
			
			if((NameEditType == 0) && ((SysStatus == SYS_EDIT_NAME)||(SysStatus == SYS_DEL_NAME)))
			{
			#ifdef LANGUAGE_HB	
				SetFlash(1,15-HBstrlen(StrTabRoom));//(10-NameEditIdx));
			#else
				SetFlash(1,strlen(Disbuf));
			#endif
			}
		}
		else
		{
			DisplayString(1,0,StrTabRoom);
			if((NameEditType == 0) && ((SysStatus == SYS_EDIT_NAME)||(SysStatus == SYS_DEL_NAME)))
			{
			#ifdef LANGUAGE_HB	
				SetFlash(1,(10-NameEditIdx));
			#else
				SetFlash(1,strlen((uint8_t *)StrTabRoom));
			#endif
			}
		}
		
		sprintf(Disbuf,"%s%s",StrTabName,pItem->Name);
		DisplayString(2,0,Disbuf);
		if((NameEditType == 1) && ((SysStatus == SYS_EDIT_NAME)||(SysStatus == SYS_DEL_NAME)))
		{
		#ifdef LANGUAGE_HB	
			i = HBstrlen(pItem->Name);
			i -= (EditTime==0?0:1);
			if(i <= 12)
			{
				SetFlash(2,(12-i));
			}
			else
			{
				SetFlash(3,(28-i));
			}
		#else
			SetFlash(2,strlen(Disbuf));
		#endif
		}
	}
}

void EditNameItemCon(void)
{
	ClearBuf((uint8_t *)&NameEditItem,sizeof(NameEditItem));
	NameEditType = 0;
	NameEditIdx = 0;
	SysStatus = SYS_EDIT_NAME;
	ShowName(&NameEditItem,false);
}
void NameEditChageType(void)
{
	uint16_t tmp;
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
				HS_memcpy((uint8_t*)&NameEditItem,(uint8_t*)&tmpItem,sizeof(NameEditItem));
			}
			NameEditIdx = strlen(NameEditItem.Name);
			break;
		case 2:
		case 3:
		case 4:
			NameEditIdx = strlen(NameEditItem.Tel[NameEditType-2]);
			break;
		case 5:
			ShowConfirmSave();
			NameEditSaved = false;
			break;
	}
}
code uint8_t HBtab[27]={0x96,0x97,0x98,0x93,0x94,0x95,0x90,0x91,0x92,0xA0,0x9F,0xA1,0x9C,0x9E,0x9D,0x99,0x9B,0x9A,0xA8,0xA9,0xAA,0xA6,0xA5,0xA7,0xA2,0xA4,0xA3};
void NameEditKey(uint8_t key)
{
//	uint16_t tmp;
//	NameItemType tmpItem;
	static uint8_t OldKey = 0xFF;
	uint8_t HBtmp;
	
	if(key == KEY_SURE)
	{
		if(NameEditType == 5)
		{
			if(NameEditSaved)
			{
				ClearBuf((uint8_t *)&NameEditItem,sizeof(NameEditItem));
				NameEditIdx = 0;
			}
			else
			{
				if((NameEditItem.Room > 0)
				//	&& (strlen(NameEditItem.Name) > 0)
					&& (strlen(NameEditItem.Tel[0]) > 0))
				{
					if(AddNameItem(&NameEditItem) != 0xFFFF)
					{
						ShowSaveOK();
					}
					else
					{
						ShowErr();
					}
					ClearBuf((uint8_t *)&NameEditItem,sizeof(NameEditItem));
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
		NameEditType %= 6;
		NameEditChageType();
		EditChar = 0;
		EditTime = 0;
		if(NameEditType == 5)
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
			NameEditType = 4;
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
			case 3:
			case 4:
				if(NameEditIdx > 0)
				{
					NameEditIdx--;
					NameEditItem.Tel[NameEditType-2][NameEditIdx] = 0;
				}
				else
				{
					NameEditType--;
					NameEditChageType();
				}
				break;
			case 5:
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
				{
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
							else if(HBtmp == 5)
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
				}
				break;
			case 2:
			case 3:
			case 4:
				if(NameEditIdx < 11)
				{
					NameEditItem.Tel[NameEditType-2][NameEditIdx++] = key+'0';
				}
				break;
		}
	}
	
	OldKey = key;
	ShowName(&NameEditItem,(NameEditType > 1));
}

void DeleteNameCon(void)
{
	ClearBuf((uint8_t *)&NameEditItem,sizeof(NameEditItem));
	NameEditType = 0;
	NameEditIdx = 0;
	SysStatus = SYS_DEL_NAME;
	ShowName(&NameEditItem,false);
}

void NameDeleteKey(uint8_t key)
{
	uint16_t tmp;
	NameItemType tmpItem;
	if(key == KEY_SURE)
	{
		if(NameEditType == 2)
		{
			if(NameEditSaved)
			{
				ClearBuf((uint8_t *)&NameEditItem,sizeof(NameEditItem));
				NameEditIdx = 0;
				NameEditType = 0;
			}
			else
			{
				DelNameItemByRoom(NameEditItem.Room);
				NameEditSaved = true;
				ShowSaveOK();
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
					HS_memcpy((uint8_t*)&NameEditItem,(uint8_t*)&tmpItem,sizeof(NameEditItem));
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
				NameEditItem.Name[0] = 0;
				tmp = NameEditItem.Room;
				NameEditIdx = 0;
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
void NameDelAllKey(uint8_t key)
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
	uint8_t Disbuf[10];
	SysStatus = SYS_SET_NAME;
	LcdClear();
	DisplayString(0,0,StrTabNameSet1);
	DisplayString(1,0,StrTabNameSet2);
	DisplayString(2,0,StrTabNameSet3);
	DisplayString(3,0,StrTabListCount);
	sprintf(Disbuf,"%u",UsrPara.NameListCount);
	DisplayString(3,15-HBstrlen(StrTabListCount)-strlen(Disbuf),Disbuf);
}
void NameSetKey(uint8_t key)
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

void ShowNameByIdx(uint16_t idx)
{
	NameItemType tmpItem;
	if(idx < UsrPara.NameListCount)
	{
		ReadNameItemByIdx(idx,&tmpItem);
		ShowName(&tmpItem,false);
	}
}

//bool CallByIdx(uint16_t idx,uint8_t telIdx)
//{
//	uint8_t Disbuf[20];
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

//bool CallByRoom(uint16_t Room,uint8_t telIdx)
//{
//	uint8_t Disbuf[20];
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

bool CallByNameItem(NameItemType *NameItem,uint8_t telIdx)
{
	uint8_t Disbuf[20];
	//printf("CallByNameItem:%bd,%s\r\n",telIdx,NameItem->Tel[telIdx]);
	if(strlen(NameItem->Tel[telIdx]) > 0)
	{
		if(GSM_Call(NameItem->Tel[telIdx]))
		{
			LcdClear();
			//sprintf(Disbuf,"%s%s",StrTabCall,NameItem->Tel[telIdx]);
			if(strlen(NameItem->Name) == 0)
			{
				sprintf(Disbuf,"%s%u",StrTabCall,NameItem->Room);
			}
			else
			{
				sprintf(Disbuf,"%s%s",StrTabCall,NameItem->Name);
			}
			DisplayString(1,0,Disbuf);
			return true;
		}
		return false;
	}
	return false;
}
#include "UART.h"
void RevNamelist(uint8_t *buf)
{
	int i,cnt;
	int j;
	NameItemType tmpItem;
	uint8_t  *p = buf;
	
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
		sscanf(p,"%d",&cnt);
	}
	printf("RevNamelist(Cnt:%d)\r\n",cnt);
	for(i=0;i<cnt;i++)
	{
		j = lookforstr(p,"R:");
		if(j != -1)
		{
			p += j+2;
			
			ClearBuf((uint8_t *)&tmpItem,sizeof(tmpItem));
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
			
			j = lookforstr(p,"T1:");
			if(j != -1)
			{
				p += j+3;
				//sscanf(p,"%s;",&tmpItem.Name);
				j = 0;
				while((*p != ';') && (j<TEL_LEN))
				{
					tmpItem.Tel[0][j] = *p++;
					j++;
				}
				tmpItem.Tel[0][j] = 0;
			}
			
			j = lookforstr(p,"T2:");
			if(j != -1)
			{
				p += j+3;
				//sscanf(p,"%s;",&tmpItem.Name);
				j = 0;
				while((*p != ';') && (j<TEL_LEN))
				{
					tmpItem.Tel[1][j] = *p++;
					j++;
				}
				tmpItem.Tel[1][j]  = 0;
			}
			
			j = lookforstr(p,"T3:");
			if(j != -1)
			{
				p += j+3;
				//sscanf(p,"%s;",&tmpItem.Name);
				j = 0;
				while((*p != ';') && (j<TEL_LEN))
				{
					tmpItem.Tel[2][j] = *p++;
					j++;
				}
				tmpItem.Tel[2][j]  = 0;
			}
			j = AddNameItem(&tmpItem);
			printf("Idx:%d-(%d); R:%u; N:%s; T1:%s; T2:%s; T3:%s;\r\n",i+1,j+1,tmpItem.Room,tmpItem.Name,tmpItem.Tel[0],tmpItem.Tel[1],tmpItem.Tel[2]);
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
