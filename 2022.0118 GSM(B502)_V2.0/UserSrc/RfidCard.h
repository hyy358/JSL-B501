#ifndef __RFIDCARD_H__
#define __RFIDCARD_H__
#include "config.h"

#define CARD_BUF_LEN 9

extern volatile uint8_t CardRt;

extern uint8_t CardCode[5];
extern bool CardReadOK;
extern bool CardGetOk;
void WG_Read26(void);
void CardReadInit(void);
void CardIrq(void);
void CardMsTask(void);
void WG_Send26(void);
void CheckRfidCard(void);
#endif
