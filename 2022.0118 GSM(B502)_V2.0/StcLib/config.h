
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

//#define MAIN_Fosc		22118400L	//定义主时钟
//#define MAIN_Fosc		12000000L	//定义主时钟
//#define MAIN_Fosc		11059200L	//定义主时钟
//#define MAIN_Fosc		 5529600L	//定义主时钟
//#define MAIN_Fosc		24000000L	//定义主时钟
#define MAIN_Fosc		27000000L	//定义主时钟


#define STC8Hxx		//STC8H系列
//#define STC8Gxx		//STC8G系列

/*********************************************************/

#include	"STC8xxxx.H"

typedef struct
{
//    unsigned char  Index[1];	// 汉字内码索引    
	unsigned char  Msk[16];		// 点阵码数据 
}typFNT_Ascii;

#endif
