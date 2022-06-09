#ifndef __AUDIO_H__
#define __AUDIO_H__
#include "config.h"

//#define MP3_MODULE
#define PWM_AUDIO
#define KEY_BEEP

typedef enum
{
    MP3_InputRoom = 16,
	MP3_InputPsw = 2,
	MP3_Unlock,
	MP3_RoomErr,
	MP3_PwdErr,
	MP3_SetOK,
	MP3_CardErr,
	MP3_Huangup,
	MP3_Noresponse,
	MP3_Ring,
	MP3_Failed,
	MP3_Calling,
	MP3_NoSimCard,
	MP3_Speak,
	MP3_CallAlt,
	MP3_InputRoomAndBel,
	MP3_Key,
	MP3_Last,
}AudioIdxype;

#ifdef MP3_MODULE

#define MP3_CMD_PWRON 0x3F
#define MP3_CMD_MEDIADOWN 0x3B
#define MP3_CMD_SETMODE 0x09
#define MP3_CMD_SETVOLUM 0x06
#define MP3_CMD_PLAY 0x03
#define MP3_CMD_RE_STOP 0x3D
#define MP3_CMD_RE_ERR 0x40

typedef struct
{
    u16 St;
	u8 Len;
    u8 Cmd;
    u8 Fb;
	u8 Para1;
    u8 Para2;
	u16 Chk;
	u8 End;
}Mp3CmdType;

void Mp3UartRx(u8 * buf);
void MP3_StartPlay(u16 idx);
#endif

#ifdef PWM_AUDIO
typedef struct
{
    uint32_t FileAddr;
	uint32_t FileLen;
}WavFileType;

extern bool AudioPlaying;
extern bool WavNeedRead;
void SPI_config(void);
void PWM_config(void);
void PWMtask(void);
void WavReadTask(void);
void MP3_StartPlay(uint8_t idx);
void MP3_Stop(void);
#endif

#ifdef KEY_BEEP
typedef struct
{
    u8 Len;
	u8 Buf[10];
}BeepFileType;
extern bit KeyBeepO;
void Beep_StartPlay(u8 idx);
void KeyBeepTask(void);
#endif

#endif
