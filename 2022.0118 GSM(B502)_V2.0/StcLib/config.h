
#ifndef		__CONFIG_H
#define		__CONFIG_H

#ifndef bool
	#define bool bit
#endif
#ifndef false
	#define false 0
#endif
#ifndef true
	#define true 1
#endif
#ifndef NULL
	#define NULL 0
#endif
	
#ifndef uint8_t
	#define uint8_t u8
#endif
#ifndef uint16_t
	#define uint16_t u16
#endif
#ifndef uint32_t
	#define uint32_t u32
#endif

/*********************************************************/

//#define MAIN_Fosc		22118400L	//������ʱ��
//#define MAIN_Fosc		12000000L	//������ʱ��
//#define MAIN_Fosc		11059200L	//������ʱ��
//#define MAIN_Fosc		 5529600L	//������ʱ��
//#define MAIN_Fosc		24000000L	//������ʱ��
#define MAIN_Fosc		27000000L	//������ʱ��


#define STC8Hxx		//STC8Hϵ��
//#define STC8Gxx		//STC8Gϵ��

/*********************************************************/

#include	"STC8xxxx.H"

typedef struct
{
//    unsigned char  Index[1];	// ������������    
	unsigned char  Msk[16];		// ���������� 
}typFNT_Ascii;

#endif
