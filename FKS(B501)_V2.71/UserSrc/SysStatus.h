#ifndef __SYS_STATUS_H__
#define __SYS_STATUS_H__
#include "config.h"
#include "NameList.h"

#define NAME_LIST_COUNT 100
#define PSW_COUNT 10
#define CARD_COUNT 500

#define PARA_INIT_FLAG 0x5A69

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
	
//		SYS_SET_TONE,
//	//	SYS_SET_TALKTIME,
//	//	SYS_SET_RINGIN,
//		SYS_SET_RINGOUT,
//	//	SYS_SET_TONE,
//		SYS_SET_RELAY,
//	//	SYS_SET_UNLOCKKEY,
//		SYS_SET_CAMERA,
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
    u16 InitFlag;
	u32 SysPsw;
    u16 TalkTime;
//    u8 RingInTs;
	u8 RingOutTs;
    u8 ToneEn;
	u8 CameraOn;
	u8 RealyTime;
//	u8 UnlockKey;
	u8 TalkVolum;
}SysParaType;

typedef struct
{
    u16 InitFlag;
    u16 NameListCount;
    u16 PswCount;
    u16 CardCount;
}UsrParaType;

extern SysParaType SysPara;
extern UsrParaType UsrPara;
extern SysStatusType SysStatus;
extern u16 NameListIdx;
extern u16 IdleTime;
extern NameItemType NameItemCall;
extern u8 CallTleIdx;

extern uint8_t Volum;

void InitSysPara(bool rst);
void InitUserPara(void);
u16 ReadRoomByIdx(u16 idx,u8 type);
u16 GetIdxByRoom(u16 room,u8 type);
void SysIdleCon(void);
void SysSettingCon(void);
void SubSetCon(SysStatusType type);
void SysKey(u8 Key);
void SysCard(u8 * card);
#endif
