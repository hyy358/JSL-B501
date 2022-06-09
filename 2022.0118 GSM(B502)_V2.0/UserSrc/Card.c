#include "Card.h"
#include "Driver.h"
#include "Eeprom.h"
#include "SysString.h"
#include "stdio.h"
#include "Lcd12864.h"
#include "SysStatus.h"
#include "NameList.h"
#include "RfidCard.h"

void SaveCardCount(void)
{
	writeBuf_at24c02(USER_PARA_ADDR+((uint16_t)((uint32_t)(&UsrPara.CardCount)-(uint32_t)(&UsrPara))), (uint8_t *)&UsrPara.CardCount,sizeof(UsrPara.CardCount));
}

void ClearCard(void)
{
	UsrPara.CardCount = 0;
	SaveCardCount();
}

void InitCard(void)
{
	if(UsrPara.CardCount > CARD_COUNT)
	{
		ClearCard();
	}
}

void WriteCardByIdx(uint16_t idx,CardItemType *pItem)
{
	uint16_t addr = (uint16_t)idx;
	addr *= sizeof(CardItemType);
	writeBuf_at24c02(USER_CARD_ADDR+addr, (uint8_t *)pItem,sizeof(CardItemType));
}

void ReadCardByIdx(uint16_t idx,CardItemType *pItem)
{
	uint16_t addr = (uint16_t)idx;
	addr *= sizeof(CardItemType);
	readBuf_at24c02(USER_CARD_ADDR+addr, (uint8_t *)pItem,sizeof(CardItemType));
}

//uint16_t GetCardByRoom(uint16_t room,CardItemType *pItem)
//{
//	uint16_t idx = 0xFFFF;
//	idx = GetIdxByRoom(room,CARD_LIST);
//	if(idx != 0xFFFF)
//	{
//		ReadCardByIdx(idx,pItem);
//		return idx;
//	}
//	return 0xFFFF;
//}

uint16_t CheckCard(uint8_t *card)
{
	CardItemType tmpItem;
	uint16_t idx = 0;
	uint8_t i;
	if(UsrPara.CardCount == 0)
	{
		return 0xFFFF;
	}
	while(idx < UsrPara.CardCount)
	{
		ReadCardByIdx(idx,&tmpItem);
		for(i=0;i<RFID_CARD_LEN;i++)
		{
			if(tmpItem.Card[i] != card[i])
			{
				break;
			}
		}
		if(i == RFID_CARD_LEN)
		{
			printf("CheckCard:%bd\r\n",idx);
			return idx;
		}
		idx++;
	}
	return 0xFFFF;
}

uint16_t AddCardItem(CardItemType *pItem)
{
	CardItemType tmpItem;
	uint16_t idx = 0;
	uint8_t i;
	while(idx < UsrPara.CardCount)
	{
		ReadCardByIdx(idx,&tmpItem);
		
		if(tmpItem.Room == pItem->Room)
		{
			for(i=0;i<RFID_CARD_LEN;i++)
			{
				if(tmpItem.Card[i] != pItem->Card[i])
				{
					break;
				}
			}
			if(i == RFID_CARD_LEN)
			{
				return idx;
			}
		}
		idx++;
	}
	if(UsrPara.CardCount < CARD_COUNT)
	{
		WriteCardByIdx(UsrPara.CardCount,pItem);
		UsrPara.CardCount++;
		SaveCardCount();
		return UsrPara.CardCount-1;
	}
	return 0xFFFF;
}

void DelCardByIdx(uint16_t idx)
{
	CardItemType tmpItem;
	if(idx < UsrPara.CardCount)
	{
		if(idx == (UsrPara.CardCount-1))
		{
			UsrPara.CardCount--;
			SaveCardCount();
		}
		else
		{
			UsrPara.CardCount--;
			SaveCardCount();
			ReadCardByIdx(UsrPara.CardCount,&tmpItem);
			WriteCardByIdx(idx,&tmpItem);
		}
	}
}

void DelCardByRoom(uint16_t room)
{
	uint16_t idx = GetIdxByRoom(room,CARD_LIST);
	while(idx != 0xFFFF)
	{
		DelCardByIdx(idx);
		idx = GetIdxByRoom(room,CARD_LIST);
	}
}

void DelCardByCard(uint8_t *card)
{
	uint16_t idx = CheckCard(card);
	while(idx != 0xFFFF)
	{
		DelCardByIdx(idx);
		idx = CheckCard(card);
	}
}


CardItemType CardEditItem;
static uint8_t CardEditMidx = 0;
static uint8_t CardEditIdx = 0;

void ShowCard(CardItemType *pItem,bool edit)
{
//	uint8_t i;
	uint32_t num;
	char Disbuf[60];
	LcdClear();
//	if(pItem->Room > 0)
//	{
//		sprintf(Disbuf,"%s%4d",StrTabNum,pItem->Room);
//		DisplayString(1,0,Disbuf);
//		if(CardEditMidx == 0)
//		{
//		#ifdef LANGUAGE_HB	
//			SetFlash(1,7);
//		#else
//			SetFlash(1,strlen(StrTabNum));
//		#endif
//		}
//	}
//	else
//	{
//		DisplayString(1,0,StrTabNum);
//		if(CardEditMidx == 0)
//		{
//		#ifdef LANGUAGE_HB	
//			SetFlash(1,7);
//		#else
//			SetFlash(1,strlen(StrTabNum));
//		#endif
//		}
//	}
	if(pItem->Room > 0)
	{
	//	sprintf(Disbuf,"%s%u",StrTabNum,pItem->Room);	
		DisplayString(1,0,StrTabNum);
		sprintf(Disbuf,"%u",pItem->Room);
		DisplayString(1,(16-HBstrlen(StrTabNum)-strlen(Disbuf)),Disbuf);
	}
	else
	{
		//sprintf(Disbuf,"%s%",StrTabNum);
		DisplayString(1,0,StrTabNum);
	}
	
	if(CardEditMidx == 0)
	{
	#ifdef LANGUAGE_HB	
		SetFlash(1,15-HBstrlen(StrTabNum));
	#else
		SetFlash(1,strlen((uint8_t *)Disbuf));
	#endif
	}
	if(edit)
	{
		//sprintf(Disbuf,"%s",StrTabCard);
		DisplayString(2,0,StrTabCard);
		if(!MemIsEmpty(pItem->Card,RFID_CARD_LEN))
		{
//			for(i=0;i<RFID_CARD_LEN;i++)
//			{
//				sprintf(Disbuf,"%s%b02X",Disbuf,pItem->Card[i]);
//			}
			num = pItem->Card[0];
			num <<= 8;
			num |= pItem->Card[1];
			num <<= 8;
			num |= pItem->Card[2];
			num <<= 8;
			num |= pItem->Card[3];
			sprintf(Disbuf,"%lu",num>>8);
			DisplayString(2,(16-HBstrlen(StrTabCard)-strlen(Disbuf)),Disbuf);
		}
		//DisplayString(2,0,Disbuf);
	}
}

void EditCardCon(void)
{
	ClearBuf((uint8_t *)&CardEditItem,sizeof(CardItemType));
	CardEditMidx = 0;
	CardEditIdx = 0;
	SysStatus = SYS_EDIT_CARD;
	ShowCard(&CardEditItem,true);
}

void CardEditKey(uint8_t key)
{
	uint16_t tmp;
	static bool Saved = false;
	if(key == KEY_SURE)
	{
		if(CardEditMidx == 1)
		{
			if(Saved)
			{
				ClearBuf((uint8_t *)&CardEditItem,sizeof(CardEditItem));
				CardEditIdx = 0;
			}
			else
			{
				if((CardEditItem.Room > 0)
					&& (!MemIsEmpty(CardEditItem.Card,RFID_CARD_LEN)))
				{
					if(AddCardItem(&CardEditItem) != 0xFFFF)
					{
						ShowSaveOK();
					}
					else
					{
						ShowErr();
					}
					ClearBuf((uint8_t *)&CardEditItem,sizeof(CardEditItem));
					Saved = true;
				}
				else
				{
					ShowErr();
				}
				return;
			}
		}
		CardEditMidx++;
		CardEditMidx %= 2;
		CardEditIdx = 0;
		switch(CardEditMidx)
		{
			case 0:
				CardEditIdx = 0;
				tmp = CardEditItem.Room;
				while(tmp > 0)
				{
					CardEditIdx++;
					tmp /= 10;
				}
				break;
			case 1:
//				ClearBuf((uint8_t *)&CardEditItem.Card,sizeof(CardEditItem.Card));
//				CardEditIdx = 0;
//				break;
//			case 2:
				ShowConfirmSave();
				Saved = false;
				return;
		}
	}
	else if(key == KEY_DEL)
	{
		switch(CardEditMidx)
		{
			case 0:
				if(CardEditIdx > 0)
				{
					CardEditIdx--;
					CardEditItem.Room /= 10;
				}
				else
				{
					CardSetCon();
					return;
				}
				break;
//			case 1:
//				if(CardEditIdx > 0)
//				{
//					CardEditIdx--;
//					CardEditItem.Card[CardEditIdx] = 0;
//				}
//				else
//				{
//					CardEditMidx = 0;
//					CardEditIdx = 0;
//					tmp = CardEditItem.Room;
//					while(tmp > 0)
//					{
//						CardEditIdx++;
//						tmp /= 10;
//					}
//				}
//				break;
			case 1:
				CardEditMidx = 0;
				CardEditIdx = 0;
				tmp = CardEditItem.Room;
				while(tmp > 0)
				{
					CardEditIdx++;
					tmp /= 10;
				}
				break;
		}
	}
	else if(key == KEY_BELL)
	{
		CardSetCon();
		return;
	}

	if((key >= 0) && (key <= 9))
	{
		switch(CardEditMidx)
		{
			case 0:
				if(CardEditIdx < 4)
				{
					CardEditItem.Room *= 10;
					CardEditItem.Room += key;
					CardEditIdx++;
					if(CardEditItem.Room == 0)
					{
						CardEditIdx = 0;
					}
				}
				break;
//			case 1:
//				if(CardEditIdx < CARD_LEN)
//				{
//					CardEditItem.Card[CardEditIdx++] = key+'0';
//				}
//				break;
		}
	}
	ShowCard(&CardEditItem,true);
}

void CardEditCardIn(uint8_t *card)
{
//	uint8_t i;
//	ClearBuf((uint8_t *)&CardEditItem.Card,sizeof(CardEditItem.Card));
//	for(i=0;i<RFID_CARD_LEN;i++)
//	{
//		sprintf(CardEditItem.Card,"%s%b02X",CardEditItem.Card,card[i]);
//	}
//	CardEditMidx = 1;
//	CardEditIdx = strlen(CardEditItem.Card);
	HS_memcpy(CardEditItem.Card,card,RFID_CARD_LEN);
	ShowCard(&CardEditItem,true);
}

void DeleteCardCon(void)
{
	ClearBuf((uint8_t *)&CardEditItem,sizeof(PswEditType));
	CardEditMidx = 0;
	CardEditIdx = 0;
	SysStatus = SYS_DEL_CARD;
	ShowCard(&CardEditItem,!MemIsEmpty(CardEditItem.Card,RFID_CARD_LEN));
}

void CardDeleteKey(uint8_t key)
{
	uint16_t tmp;
	static bool Saved = false;
	if(key == KEY_SURE)
	{
		if(CardEditMidx == 1)
		{
			if(Saved)
			{
				ClearBuf((uint8_t *)&CardEditItem,sizeof(CardEditItem));
				CardEditIdx = 0;
			}
			else
			{
				if(!MemIsEmpty(CardEditItem.Card,RFID_CARD_LEN))
				{
					DelCardByCard(CardEditItem.Card);
					ClearBuf((uint8_t *)&CardEditItem,sizeof(CardEditItem));
					Saved = true;
					ShowDeleteOK();
				}
				else if(CardEditItem.Room > 0)
				{
					DelCardByRoom(CardEditItem.Room);
					ClearBuf((uint8_t *)&CardEditItem,sizeof(CardEditItem));
					Saved = true;
					ShowDeleteOK();
				}
				return;
			}
		}
		CardEditMidx++;
		CardEditMidx %= 2;
		CardEditIdx = 0;
		switch(CardEditMidx)
		{
			case 0:
				CardEditIdx = 0;
				tmp = CardEditItem.Room;
				while(tmp > 0)
				{
					CardEditIdx++;
					tmp /= 10;
				}
				break;
			case 1:
				ShowConfirmDel();
				Saved = false;
				return;
		}
	}
	else if(key == KEY_DEL)
	{
		switch(CardEditMidx)
		{
			case 0:
				if(CardEditIdx > 0)
				{
					CardEditIdx--;
					CardEditItem.Room /= 10;
				}
				else
				{
					CardSetCon();
					return;
				}
				break;
			case 1:
				CardEditMidx = 0;
				CardEditIdx = 0;
				tmp = CardEditItem.Room;
				while(tmp > 0)
				{
					CardEditIdx++;
					tmp /= 10;
				}
				break;
		}
	}
	else if(key == KEY_BELL)
	{
		CardSetCon();
		return;
	}

	if((key >= 0) && (key <= 9))
	{
		switch(CardEditMidx)
		{
			case 0:
				if(CardEditIdx < 4)
				{
					CardEditItem.Room *= 10;
					CardEditItem.Room += key;
					CardEditIdx++;
				}
				if(CardEditItem.Room == 0)
				{
					CardEditIdx = 0;
				}
				break;
		}
	}
	ShowCard(&CardEditItem,!MemIsEmpty(CardEditItem.Card,RFID_CARD_LEN));
}

void DeleteAllCardCon(void)
{
	SysStatus = SYS_DEL_ALLCARD;
	LcdClear();
	DisplayString(1,0,StrTabDelAllCard);
}
void CardDelAllKey(uint8_t key)
{
	switch(key)
	{
		case KEY_SURE:
			ClearCard();
			ShowDeleteOK();
			break;
		case KEY_BELL:
		case KEY_DEL:
			CardSetCon();
			break;
	}
}

void CardSetCon(void)
{
	uint8_t Disbuf[10];
	
	SysStatus = SYS_SET_CARD;
	
	LcdClear();
	DisplayString(0,0,StrTabCardSet1);
	DisplayString(1,0,StrTabCardSet2);
	DisplayString(2,0,StrTabCardSet3);
	DisplayString(3,0,StrTabListCount);
	sprintf(Disbuf,"%u",UsrPara.CardCount);
	DisplayString(3,15-HBstrlen(StrTabListCount)-strlen(Disbuf),Disbuf);
}

void CardSetKey(uint8_t key)
{
	switch(key)
	{
		case 1:
			EditCardCon();
			break;
		case 2:
			DeleteCardCon();
			break;
		case 3:
			DeleteAllCardCon();
			break;
		case KEY_DEL:
		case KEY_BELL:
			SysSettingCon();
			break;
	}
}
