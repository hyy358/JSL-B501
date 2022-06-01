#include "SysString.h"
#include "Lcd12864.h"
#include "stdio.h"
#include "Audio.h"

code uint8_t FhChar[]={"\",/:()'_- "};

#ifdef LANGUAGE_EN
code char StrTabSysSet[] = {"Sys Setting"};
code char StrTabSysSet1[] = {"1-8"};

code char StrTabNameSet1[] = {"1-Add/Edit Name"};
code char StrTabNameSet2[] = {"2-Delete Name"};
code char StrTabNameSet3[] = {"3-Clear all Name"};
code char StrTabDelAllName[] = {"Clear all Name ?"};
code char StrTabRoom[] = {"Room:"};
code char StrTabName[] = {"Name:"};
code char StrTabTel[] = {"Relay:"};
code char StrTabMakeSure[] = {" Confirm save ?"};
code char StrTabSaveOK[] = {"   Save OK !"};
code char StrTabErr[] = {"  Error !"};

code char StrTabDel[] = {"Confirm delete ?"};
code char StrTabDelOK[] = {"   Deleted !"};
code char StrTabDelAllPsw[] = {"Clear all PSW ?"};
code char StrTabPswSet1[] = {"1-Add PSW"};
code char StrTabPswSet2[] = {"2-Delete PSW"};
code char StrTabPswSet3[] = {"3-Clear all PSW"};
code char StrTabPsw1[] = {"Psw:"};
code char StrTabPsw2[] = {"Confirm:"};

code char StrTabListCount[] = {"List Count:"};
code char StrTabSysPsw[] = {"Admin PSW"};

code char StrTabCardSet1[] = {"1-Add Card"};
code char StrTabCardSet2[] = {"2-Delete Card"};
code char StrTabCardSet3[] = {"3-Clear all Card"};
code char StrTabDelAllCard[] = {"Clear all Card ?"};
code char StrTabCard[] = {"Card:"};
code char StrTabCardErr[] = {"  Card Err!"};
code char StrTabUnlock[] = {"  Door Opened!"};

code char StrTabCall[] = {"Call:"};

code char StrTabTalkTime[] = {"TalkTime:"};
code char StrTabRingTime[] = {"RingTime:"};
code char StrTabCallTime[] = {"CallTime:"};
code char StrTabTone[] = {"Tone:"};
code char StrTabCamrea[] = {"Camrea:"};
code char StrTabRelay[] = {"Relay:"};

code char StrTabUnlockKey[] = {"UnlockKey:"};

code char StrTabOn[] = {"on"};
code char StrTabOff[] = {"off"};

code char StrTabGsmErr[] = {"  Gsm Err!"};
//code char StrTabGsmCsq[] = {"Csq:"};
//code char StrTabGsmCsqErr[] = {"No Signal"};

code char StrTabMain1[] = {"Name List"};
code char StrTabMain2[] = {"to call"};
code char StrTabMain3[] = {"XXXX   Call Room"};
code char StrTabMain4[] = {"Unlock"};

code char StrTabRoomErr[] = {"  Room Err!"};
code char StrTabTalk[] = {"Talking:"};

code char StrTabReset[] = {"Reset OK!"};
#endif

#ifdef LANGUAGE_HB
code char StrTabSysSet[] = {"הגדרת מערכת"};
code char StrTabSysSet1[] = {"1-13"};

code char StrTabNameSet1[] = {"1-הוסף/ערוך שם"};
code char StrTabNameSet2[] = {"2-מחק שם"};
code char StrTabNameSet3[] = {"3- מחק הכל"};
code char StrTabDelAllName[] = {"האם למחוק הכל ?"};
code char StrTabRoom[] = {"דירה:"};
code char StrTabNum[] = {"תא: "};
code char StrTabName[] = {"שם:"};
code char StrTabTel[] = {"ממסר:"};
code char StrTabMakeSure[] = {" לאשר שמירה?"};
code char StrTabSaveOK[] = {"   שמור!"};
code char StrTabErr[] = {"  שגיאה!"};

code char StrTabDel[] = {"לאשר מחיקה ?"};
code char StrTabDelOK[] = {"  נמחק!"};
code char StrTabDelAllPsw[] = {"האם למחוק הכל ?"};
code char StrTabPswSet1[] = {"1- הוסף סיסמה"};
code char StrTabPswSet2[] = {"2-מחק סיסמה"};
code char StrTabPswSet3[] = {"3-מחק הכל"};
code char StrTabPsw1[] = {"סיסמה:"};
code char StrTabPsw2[] = {"אשר:"};

code char StrTabListCount[] = {"סך הכל:"};
code char StrTabSysPsw[] = {"סיסמת מנהל"};

code char StrTabCardSet1[] = {"1-הוסף כרטיס"};
code char StrTabCardSet2[] = {"2-מחיקת כרטיס"};
code char StrTabCardSet3[] = {"3-מחק הכל"};
code char StrTabDelAllCard[] = {"האם למחוק הכל ?"};
code char StrTabCard[] = {"כרטיס:"};
code char StrTabCardErr[] = {"  שגיאת כרטיס!"};
code char StrTabUnlock[] = {"  דלת נפתחת!"};

code char StrTabCall[] = {"מצלצל:"};

code char StrTabTalkTime[] = {"זמן דיבור:"};
code char StrTabRingTime[] = {"זמן צלצול:"};
code char StrTabCallTime[] = {"זמן צלצול:"};
code char StrTabTone[] = {"הנחיות קוליות:"};
code char StrTabCamrea[] = {"מצלמה:"};
code char StrTabRelay[] = {"זמן ממסר:"};
code char StrTabUnlockKey[] = {"מקש פ.דלת:"};
code char StrTabTalkValum[] = {"עוצמת דיבור"};

code char StrTabOn[] = {"פעיל"};
code char StrTabOff[] = {"כבוי"};

code char StrTabGsmErr[] = {"שגיאת כרטיס סים!"};
//code char StrTabGsmCsq[] = {"Csq:"};
//code char StrTabGsmCsqErr[] = {"אין קליטה"};

code char StrTabMainA1[] = {"ברוכים הבאים!"};
code char StrTabMainA2[] = {"לצלצול לדירה"};
code char StrTabMainA3[] = {"לחצו מספר דירה"};
code char StrTabMainA4[] = {"לביטול לחצו"};

code char StrTabMain1[] = {"לרשימת שמות"};
code char StrTabMain2[] = {" לבחירת שם לחצו"};
code char StrTabMain3[] = {"לכניסה עם קוד"};
code char StrTabMain4[] = {"לחצו   וקוד"};

code char StrTabRoomErr[] = {" שגיאה בדירה!"};
code char StrTabTalk[] = {"שיחה:"};

code char StrTabReset[] = {"Reset OK!"};
#endif
//code char StrTabGsmCsqErr[] = {"אבגדהוזחטיךכלםמןנסעףפץצקרשת"};
//code char StrTabMain1[] = {"אבגדהוזחטיךכלםמן"};
//code char StrTabMain2[] = {"נסעףפץצקרשת"};

char DisBuf[20];

void ShowConfirmSave(void)
{
	LcdClear();
	DisplayString(1,0,StrTabMakeSure);
}
void ShowConfirmDel(void)
{
	LcdClear();
	DisplayString(1,0,StrTabDel);
}
void ShowSaveOK(void)
{
	LcdClear();
	DisplayString(1,0,StrTabSaveOK);
	MP3_StartPlay(MP3_SetOK);
}
void ShowErr(void)
{
	LcdClear();
	DisplayString(1,0,StrTabErr);
}
void ShowDeleteOK(void)
{
	LcdClear();
	DisplayString(1,0,StrTabDelOK);
}

void ShowCardErr(void)
{
	LcdClear();
	DisplayString(1,0,StrTabCardErr);
	MP3_StartPlay(MP3_CardErr);
}
void ShowUnlock(bool Sound)
{
	LcdClear();
	DisplayString(1,0,StrTabUnlock);
	if(Sound)
	{
		MP3_StartPlay(MP3_Unlock);
	}
}

//void ShowGsmErr(void)
//{
//	LcdClear();
//	DisplayString(1,0,StrTabGsmErr);
//}

void ShowRoomErr(void)
{
	LcdClear();
	DisplayString(1,0,StrTabRoomErr);
}

void ShowGsmCsq(void)
{
//	if((GsmCsq == 0xff) || (GsmCsq < 5))
//	{
//		//DisplayString(0,0,StrTabGsmCsqErr);
//		DisplayCsq(0,0,0);
//		DisplayIco(0,1,6);
//		return;
//	}
//	DisplayCsq(0,0,GsmCsq);
}

void ShowMain(u8 first)
{
	LcdClear();
	ShowGsmCsq();
#ifdef LANGUAGE_HB		
	if(first == 0)
	{
		DisplayString(0,0,StrTabMainA1);
		DisplayString(1,0,StrTabMainA2);
		DisplayString(2,0,StrTabMainA3);
		DisplayIco(2,0,2);
		DisplayString(3,0,StrTabMainA4);
		DisplayIco(3,3,8);
	}
	else
	{
		DisplayString(0,0,StrTabMain1);
		DisplayIco(0,2,0);
		DisplayIco(0,3,1);
		
		DisplayString(1,0,StrTabMain2);
		DisplayIco(1,0,2);
		
		DisplayString(2,0,StrTabMain3);
		
		DisplayString(3,0,StrTabMain4);
		DisplayIco(3,10,3);
	}
#endif
#ifdef LANGUAGE_EN		
	DisplayString(0,5,StrTabMain1);
	DisplayIco(0,2,0);
	DisplayIco(0,3,1);
	
	DisplayString(1,4,StrTabMain2);
	DisplayIco(1,2,2);
	
	
	DisplayString(2,0,StrTabMain3);
	DisplayIco(2,5,2);
	
	DisplayString(3,3,StrTabMain4);
	DisplayIco(3,1,3);

	
#endif
}

void ShowTalkTime(void)
{
	LcdClear();
#ifdef LANGUAGE_HB
	DisplayString(1,0,StrTabTalk);
	sprintf(DisBuf,"%u",CallOutTime);
	DisplayString(1,8,DisBuf);
#else
	sprintf(DisBuf,"%s%u",StrTabTalk,CallOutTime);
	DisplayString(1,0,DisBuf);
#endif
}
void ShowReset(void)
{
	LcdClear();
	DisplayString(1,0,StrTabReset);
}
