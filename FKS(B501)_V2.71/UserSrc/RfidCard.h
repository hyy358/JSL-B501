#ifndef __RFIDCARD_H__
#define __RFIDCARD_H__
#include "config.h"

#define CARD_BUF_LEN 9
#define RFID_CARD_LEN 4

//struct wiegand_dev
//{
//  uint8_t wiegand[26];                             // 定义Weigand  26Bits 数据
//  uint16_t  state;  
//  uint16_t global_var;                                 // 全局计数器 计数组长度
//};static struct wiegand_dev *rf_card;


extern volatile uint8_t CardRt;

extern uint8_t CardCode[RFID_CARD_LEN];
extern bool CardReadOK;
extern bool CardGetOk;
extern void WG_Send26(uint8_t *str);
void WGOK(void);
void CardReadInit(void);
void DATA0(void);
void DATA1(void);
void CardIrq(void);
void CardMsTask(void);
void CheckRfidCard(void);
#endif
