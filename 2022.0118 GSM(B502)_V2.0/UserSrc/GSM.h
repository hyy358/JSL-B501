#ifndef __GSM_H__
#define __GSM_H__
#include "Driver.h"

typedef enum
{
    GSM_ST_REPWR = 0,
	GSM_ST_KEYH,
	GSM_ST_KEYL,
	GSM_ST_AT,
	GSM_ST_WAITAT,
	GSM_ST_ATOVER,
	GSM_ST_CALL,
	GSM_ST_CALLIN,
	GSM_ST_TALK,
	GSM_ST_WAITHUNGUP,
	GSM_ST_PWRDOWN,
	GSM_ST_UNKNOW = 0xff,

}GsmStatusType;

typedef enum
{
    AT_RSP_ERROR = -1,
    AT_RSP_WAIT= 0, 
    AT_RSP_CONTINUE = 1,                        /* ����ִ����һ��AT����������� */
    AT_RSP_PAUSE= 2,                            /* ��ִͣ��AT�������� */
    AT_RSP_FINISH = 3,                          /* ִֹͣ��AT�������� */

    AT_RSP_FUN_OVER = 4,                        /* ����ģ�黯AT������ִ����ϣ����԰ѱ������еĹ������ */
    AT_RSP_STEP_MIN = 10,
    AT_RSP_STEP = 20,                           /* ����ִ�б���AT���� */
    AT_RSP_STEP_MAX = 30,

}AtCmdRspType;
typedef AtCmdRspType (*AtCmdRspCB)(char *pRspStr);
typedef struct
{
    uint8_t* p_atCmdStr;
    AtCmdRspCB p_atCmdCallBack;

}AtCmdEntity;

extern uint8_t GsmCsq;
extern bool GsmCsqFlag;
extern uint16_t CallOutTime;
extern uint16_t TalkTime;
extern uint8_t PaOpenTime;

void GSM_StartInit(void);
void GSM_InitTask(void);
void GsmUARTCmd(unsigned char * buf,uint16_t len);
bool GSM_Call(uint8_t * tel);
void GSM_Huangup(void);
void GSM_SetVolum(uint8_t volum);
void GSM_send_dtmf(uint8_t k);
#endif
