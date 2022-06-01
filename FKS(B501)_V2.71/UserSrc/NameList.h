#ifndef __NAMELIST_H__
#define __NAMELIST_H__
#include "config.h"

#define TEL_LEN 3
#define NAME_LEN 20

typedef struct
{
    u16 Room;
    u8 Name[NAME_LEN*2+2];
	u16 Tel;
}NameItemType;

extern u16 EditTime;
void InitNameList(void);
void ReadNameItemByIdx(u16 idx,NameItemType *pItem);
u16 GetNameItemByRoom(u16 room,NameItemType *pItem);
void ShowName(NameItemType *pItem,bool tel);
void EditNameItemCon(void);
void NameEditKey(u8 key);

void DeleteNameCon(void);
void NameDeleteKey(u8 key);
void DeleteAllNameCon(void);
void NameDelAllKey(u8 key);
void NameSetCon(void);
void NameSetKey(u8 key);

void ShowNameByIdx(u16 idx);
//bool CallByIdx(u16 idx,u8 telIdx);
//bool CallByRoom(u16 Room,u8 telIdx);
bool CallByNameItem(NameItemType *NameItem);
void RevNamelist(u8 *buf);
#endif
