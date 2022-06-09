#ifndef __SYS_STATUS_H__
#define __SYS_STATUS_H__
#include "Driver.h"
#include "NameList.h"

#define NAME_LIST_COUNT 100
#define PSW_COUNT 10
#define CARD_COUNT 500

#define PARA_INIT_FLAG 0x5A63

typedef enum
{
    SYS_IDLE = 1,
	SYS_SETTING,
	
	SYS_SET_NAME,
	SYS_EDIT_NAME,
	SYS_DEL_NAME,
	SYS_DEL_ALLNAME,
	SYS_INPUT_ROOM,
	
	SYS_SET_PSW,
	SYS_EDIT_PSW,
	SYS_DEL_PSW,
	SYS_DEL_ALLPSW,
	SYS_INPUT_PSW,
	SYS_EDIT_SYSPSW,
	SYS_INPUT_SYSPSW,
	
	SYS_SET_CARD,
	SYS_EDIT_CARD,
	SYS_DEL_CARD,
	SYS_DEL_ALLCARD,
	
	SYS_CALL,
	SYS_TALK,
	
	SYS_SET_TALKTIME,
	SYS_SET_RINGIN,
	SYS_SET_RINGOUT,
	SYS_SET_TONE,
	SYS_SET_RELAY,
	SYS_SET_UNLOCKKEY,
	SYS_SET_UNLOCKKEY2,
	SYS_SET_CAMERA,
	SYS_SET_TALKVOLUM,
}SysStatusType;

typedef enum
{
    NAME_LIST = 1,
	UNLOCK_PSW,
	CARD_LIST,
}EromReadType;

typedef struct
{
    uint16_t InitFlag;
    uint16_t TalkTime;
    uint8_t RingInTs;
	uint8_t RingOutTs;
    uint8_t ToneEn;
	uint8_t CameraOn;
	uint8_t RealyTime;
	uint8_t UnlockKey;
	uint8_t UnlockKey2;
	uint8_t TalkVolum;
	uint32_t SysPsw;
}SysParaType;

typedef struct
{
    uint16_t InitFlag;
    uint16_t NameListCount;
    uint16_t PswCount;
    uint16_t CardCount;
}UsrParaType;

extern SysParaType SysPara;
extern UsrParaType UsrPara;
extern SysStatusType SysStatus;
extern uint16_t NameListIdx;
extern uint16_t IdleTime;
extern NameItemType NameItemCall;
extern uint8_t CallTleIdx;

extern uint8_t MainShow;

extern uint8_t Volum;

void InitSysPara(bool rst);
void InitUserPara(void);
uint16_t ReadRoomByIdx(uint16_t idx,EromReadType type);
uint16_t GetIdxByRoom(uint16_t room,EromReadType type);
void SysIdleCon(void);
void SysSettingCon(void);
void SubSetCon(SysStatusType type);
void SysKey(uint8_t Key);
void SysCard(uint8_t * card);
#endif
