#ifndef __UCARD_H__
#define __UCARD_H__
#include "Driver.h"
#include "RfidCard.h"
#define RFID_CARD_LEN 4
typedef struct
{
    uint16_t Room;
    uint8_t Card[RFID_CARD_LEN];
}CardItemType;

uint16_t CheckCard(uint8_t *card);
void InitCard(void);
void CardEditKey(uint8_t key);
void CardDeleteKey(uint8_t key);
void CardDelAllKey(uint8_t key);
void CardSetCon(void);
void CardSetKey(uint8_t key);
void CardEditCardIn(uint8_t *card);

#endif
