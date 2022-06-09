#ifndef __NAMELIST_H__
#define __NAMELIST_H__
#include "Driver.h"

#define TEL_COUNT 3
#define TEL_LEN 12
#define NAME_LEN 20
typedef struct
{
    uint16_t Room;
    uint8_t Name[NAME_LEN*2+2];
	uint8_t Tel[TEL_COUNT][TEL_LEN];
}NameItemType;

extern uint16_t EditTime;
void InitNameList(void);
void ReadNameItemByIdx(uint16_t idx,NameItemType *pItem);
uint16_t GetNameItemByRoom(uint16_t room,NameItemType *pItem);
void ShowName(NameItemType *pItem,bool tel);
void EditNameItemCon(void);
void NameEditKey(uint8_t key);

void DeleteNameCon(void);
void NameDeleteKey(uint8_t key);
void DeleteAllNameCon(void);
void NameDelAllKey(uint8_t key);
void NameSetCon(void);
void NameSetKey(uint8_t key);

void ShowNameByIdx(uint16_t idx);
//bool CallByIdx(u16 idx,u8 telIdx);
//bool CallByRoom(u16 Room,u8 telIdx);
bool CallByNameItem(NameItemType *NameItem,uint8_t telIdx);
void RevNamelist(uint8_t *buf);
#endif
