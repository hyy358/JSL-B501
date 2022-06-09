#ifndef __UNLOCK_PASSWORD_H__
#define __UNLOCK_PASSWORD_H__
#include "Driver.h"

typedef struct
{
    uint16_t Room;
    uint16_t Psw;
}PswItemType;

typedef struct
{
    uint16_t Room;
    uint32_t Psw1;
	uint32_t Psw2;
}PswEditType;

void InitPsw(void);
void PswEditKey(uint8_t key);
void PswDeleteKey(uint8_t key);
void PswDelAllKey(uint8_t key);
void PswSetCon(void);
void PswSetKey(uint8_t key);
void InputPswCon(void);
void PswInputKey(uint8_t key);

void EditSysPswCon(void);
void SysPswEditKey(uint8_t key);

#endif
