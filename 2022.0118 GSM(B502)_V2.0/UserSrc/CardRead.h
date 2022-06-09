#ifndef __RFIDCARD_H__
#define __RFIDCARD_H__
#include "config.h"

#define CARD_BUF_LEN 11
#define RFID_CARD_LEN 5

extern volatile u8 CardRt;

extern u8 CardCode[RFID_CARD_LEN];
extern bool CardReadOK;
extern bool CardGetOk;
void CardReadInit(void);
void CardIrq(void);
void CheckRfidCard(void);
#endif
