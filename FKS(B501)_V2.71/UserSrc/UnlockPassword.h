#ifndef __UNLOCK_PASSWORD_H__
#define __UNLOCK_PASSWORD_H__
#include "config.h"

typedef struct
{
    u16 Room;
    u16 Psw;
}PswItemType;

typedef struct
{
    u16 Room;
    u32 Psw1;
	u32 Psw2;
}PswEditType;

void InitPsw(void);
void PswEditKey(u8 key);
void PswDeleteKey(u8 key);
void PswDelAllKey(u8 key);
void PswSetCon(void);
void PswSetKey(u8 key);
void InputPswCon(void);
void PswInputKey(u8 key);

void EditSysPswCon(void);
void SysPswEditKey(u8 key);

#endif
