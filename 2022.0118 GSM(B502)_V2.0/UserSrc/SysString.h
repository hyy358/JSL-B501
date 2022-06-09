#ifndef __SYS_STRING_H__
#define __SYS_STRING_H__
#include "Driver.h"

//#define LANGUAGE_EN
#define LANGUAGE_HB

extern code uint8_t FhChar[];

extern code char StrTabSysSet[];
extern code char StrTabSysSet1[];

extern code char StrTabNameSet1[];
extern code char StrTabNameSet2[];
extern code char StrTabNameSet3[];
extern code char StrTabDelAllName[];
extern code char StrTabRoom[];
extern code char StrTabNum[];
extern code char StrTabName[];
extern code char StrTabTel[];
extern code char StrTabMakeSure[];
extern code char StrTabSaveOK[];
extern code char StrTabErr[];
extern code char StrTabPsw1[];
extern code char StrTabPsw2[];

extern code char StrTabListCount[];
extern code char StrTabSysPsw[];

extern code char StrTabDelAllPsw[];
extern code char StrTabPswSet1[];
extern code char StrTabPswSet2[];
extern code char StrTabPswSet3[];
extern code char StrTabCall[];


extern code char StrTabTalkTime[];
extern code char StrTabRingTime[];
extern code char StrTabCallTime[];
extern code char StrTabTone[];
extern code char StrTabCamrea[];
extern code char StrTabRelay[];
extern code char StrTabUnlockKey[];
extern code char StrTabUnlockKey2[];
extern code char StrTabTalkValum[];

extern code char StrTabOn[];
extern code char StrTabOff[];

extern code char StrTabCardSet1[];
extern code char StrTabCardSet2[];
extern code char StrTabCardSet3[];
extern code char StrTabDelAllCard[];
extern code char StrTabCard[];
extern code char StrTabCardErr[];

void ShowSaveOK(void);
void ShowErr(void);
void ShowConfirmSave(void);
void ShowConfirmDel(void);
void ShowDeleteOK(void);
void ShowCardErr(void);
void ShowUnlock(bool Sound);
void ShowGsmErr(void);
void ShowRoomErr(void);
void ShowGsmCsq(void);
void ShowMain(uint8_t first);
void ShowTalkTime(void);
void ShowReset(void); 
#endif
