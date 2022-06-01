#ifndef __EEPROM_H__
#define __EEPROM_H__
#include "Driver.h"
#include "NameList.h"
#include "UnlockPassword.h"
#include "Card.h"

#define EEPROM_SIZE (512*1024/8)
#define SYS_PARA_ADDR	0
#define USER_PARA_ADDR	128
#define NAMELIST_ADDR	256
#define USER_PSW_ADDR	(NAMELIST_ADDR+(NAME_LIST_COUNT*sizeof(NameItemType)))
#define USER_CARD_ADDR	(USER_PSW_ADDR+(PSW_COUNT*sizeof(PswItemType)))
#define EEPROM_ADDR_END	(USER_CARD_ADDR+(CARD_COUNT*sizeof(CardItemType)))

//#if (EEPROM_ADDR_END > EEPROM_SIZE)
//#error "EEPROM_ADDR_END over !"
//#endif

void writeBuf_at24c02(uint16_t address, uint8_t *buf,uint16_t len);
uint8_t readBuf_at24c02(uint16_t address, uint8_t *buf,uint16_t len);

#endif
