#include "Driver.h"
#include "RfidCard.h"
#include "stdio.h"

#define uchar unsigned char
#define uint unsigned int 
uchar dat[26];
char i;
sbit rs=P2^5;
sbit en=P2^7;
sbit rw=P2^6;  
uchar num; 

//void delay(uint z)
//{ uint x,y;
//for(x=z;x>0;x--)
//for(y=110;y>0;y--);


//}

void write_com(uchar com)
{
 rs=0;
 rw=0;
 P0=com;
 delay(2);
 en=0;
 delay(2);
 en=1;
 delay(1);
}


void write_date(uchar date)
{
 rs=1;
 rw=0;
 P0=date;
 delay(2);
 en=0;
 delay(2);
 en=1;
 delay(1);
}

void write_num(uchar date)
{
 if(date<10)
 {
  write_date(0x30+date);//数转为字符
 } 
}
void init()
{
	 write_com(0x38);
	 write_com(0x38);
	 write_com(0x0c);
	 write_com(0x06);
	 write_com(0x01);
	 delay(1);	
}
   
//void main(void)   
//{  
// 	 init();
//     TMOD=0x20;
//	 TL1=0xfd;
//	 TH1=0xfd;
// 	 TR1=1;
//	 SCON=0xd0;
//	 num=0;        
//	 en=0;
//	 EA=1;
//	 EX0=1;
//	 EX1=1;
//	 IT0=1;//下降沿触发
//	 IT1=1;
// 
// while(1)
// {
//  if(num>25)
//  {
//   EA=0;

//for(i=25;i>=0;i--)
//{
//			SBUF=0x30+dat[i];
//			while(TI!=1);TI=0;
//}
//   write_com(0x01);
//   write_com(0x80);
//   for(i=25;i>9;i--)
//   write_num(dat[i]);
//   write_com(0x80+0x40);
//   for(i=9;i>=0;i--)
//   write_num(dat[i]);
//   num=0;
//   EA=1;
//  }
// }          
//}

void D0() interrupt 0
{
	 EX0=0;
	 dat[num]=0;
	 num++;
	 EX0=1;
}  

void D1() interrupt 2
{
	 EX1=0;
	 dat[num]=1;
	 num++;
	 EX1=1;
} 
