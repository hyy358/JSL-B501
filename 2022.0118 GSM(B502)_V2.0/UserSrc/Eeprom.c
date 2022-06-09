#include "Driver.h"

#define write_c02 0xa0
#define read_c02 0xa1
#define EEPROM_PAGESIZE 64

#define IIC_delay() {_nop_();_nop_();_nop_();_nop_();_nop_();}
//void IIC_delay()
//{
//	u16 i=1;
//	while(i--)_nop_();
//}
void IIC_delayms(uint8_t t)
{
	uint8_t j;
	while(t--)
		for(j=0;j<110;j++);
} 
void IIC_start() //启动i2c
{
	IIC_DAT_H;
	IIC_delay();
	IIC_CLK_H;
	IIC_delay();
	IIC_DAT_L;
	IIC_delay();
}

void IIC_stop() //停止i2c
{
	IIC_DAT_L;
	IIC_delay();
	IIC_CLK_H;
	IIC_delay();
	IIC_DAT_H; //在scl为高电平时，sdasda一个上升沿为停止信号
	IIC_delay();
}
//ack:
void IIC_ack() //应答信号0
{
	uint8_t i = 0; //等待变量
	IIC_CLK_H;
	IIC_delay();
	while((IIC_DAT == 1) && i < 250)i++;//等待一段时间//若为应答0即退出，从机向主机发送应答信号
	IIC_CLK_L; //应答之后将scl拉低
	IIC_delay();
}
//nack:
void IIC_nack() //非应答
{
	IIC_DAT_H;
	IIC_CLK_L;//在scl为高电平期间，由主机向从机发送一个1，非应答信号
	IIC_delay();
	IIC_CLK_H; //应答之后将scl拉低
	IIC_delay();
	IIC_CLK_L;
	IIC_DAT_L;
}

void IIC_Aack() //应答
{
	IIC_DAT_L;
	IIC_CLK_L;//在scl为高电平期间，由主机向从机发送一个1，非应答信号
	IIC_delay();
	IIC_CLK_H; //应答之后将scl拉低
	IIC_delay();
	IIC_CLK_L;
}


//send byte:
void IIC_send_byte(uint8_t date)//写一个字节
{
	uint8_t i, temp;
	temp = date; //存入要写入的数据，即要发送到sda上的数据
	for(i = 0; i < 8; i++)
	{ //发送8位
		IIC_CLK_L;
		IIC_delay();
		IIC_DAT = ((temp&0x80)==0x80)?1:0;
		temp <<= 1;
		IIC_delay();
		IIC_CLK_H; //在scl为高电平时，不允许sda上的数据变化，使数据稳定
		IIC_delay();
		IIC_CLK_L;
	}
	IIC_CLK_L;
	IIC_delay();
	IIC_DAT_H;
	IIC_delay();

	//IIC_ack();
}
//read: byte
uint8_t IIC_read_byte() //读一个字节数据
{
	uint8_t i,k;
	IIC_CLK_L;
	IIC_delay();
	IIC_DAT_H; //允许sda变化
	IIC_delay(); //使sda数据稳定后开始读数据
	for(i = 0; i < 8; i++)
	{
		IIC_CLK_H;
		IIC_delay();
		k <<= 1;
		k |= IIC_DAT;
		IIC_CLK_L;
		IIC_delay();
	}
	return k;//返回读出的数据
}
//write:at24c02  在at24c02中的指定地址写入数据
//void write_at24c02(u16 address, u8 date)
//{
//	IIC_WP_L;
//	IIC_delay();
//	IIC_start(); //启动i2c
//	IIC_send_byte(write_c02);//写入期间地址和写操作
//	IIC_ack(); //从机应答0
//	IIC_send_byte((address>>8)&0xFF); //写入写数据的单元地址
//	IIC_ack();
//	IIC_send_byte(address&0xFF); //写入写数据的单元地址
//	IIC_ack(); //ack0
//	IIC_send_byte(date); //在指定地址中写入数据
//	IIC_ack(); //从机应答0
//	IIC_stop();    //停止i2c
//	//IIC_Wp_H;
//}
////read: at24c02   在at24c02的指定地址中读出写入的数据
//u8 read_at24c02(u16 address)
//{
//	u8 dat;//用来存储读出的数据
//	IIC_start(); //启动i2c
//	IIC_send_byte(write_c02); //写入at24c02器件地址和写操作
//	IIC_ack(); //从机应答0
//	IIC_send_byte((address>>8)&0xFF);
//	IIC_ack(); 
//	IIC_send_byte(address&0xFF);
//	IIC_ack(); //从机应答0
//	IIC_start();  //再次启动i2c
//	IIC_send_byte(read_c02); //写入AT24C02器件地址和读操作
//	IIC_ack();//从机应答‘0’
//	dat = IIC_read_byte();//读出指定地址中的数据
//	//IIC_nack(); //主机发出非应答‘1’
//	IIC_stop();  //停止i2c
// return dat;    //返回读出的数据
//}

void writeBuf_at24c02(uint16_t address, uint8_t *buf,uint16_t len)
{
	IIC_WP_L;
	IIC_start(); //启动i2c
	IIC_send_byte(write_c02);//写入期间地址和写操作
	IIC_ack();
	IIC_send_byte((address>>8)&0xFF);
	IIC_ack();
	IIC_send_byte(address&0xFF);
	IIC_ack();
	while(len--)
	{
		IIC_send_byte(*buf++); //在指定地址中写入数据
		IIC_ack();
		address++;
		if(((address%EEPROM_PAGESIZE) == 0) && (len > 0))
		{
			IIC_stop();
			IIC_delayms(100);
			IIC_start(); //启动i2c
			IIC_send_byte(write_c02);//写入期间地址和写操作
			IIC_ack();
			IIC_send_byte((address>>8)&0xFF);
			IIC_ack();
			IIC_send_byte(address&0xFF);
			IIC_ack();
		}
	}
	IIC_stop();    //停止i2c
	IIC_delayms(100);
}

uint8_t readBuf_at24c02(uint16_t address, uint8_t *buf,uint16_t len)
{
	uint8_t dat;//用来存储读出的数据
	IIC_start(); //启动i2c
	IIC_send_byte(write_c02); //写入at24c02器件地址和写操作
	IIC_ack();
	IIC_send_byte((address>>8)&0xFF);
	IIC_ack();
	IIC_send_byte(address&0xFF);
	IIC_ack();
	IIC_start();  //再次启动i2c
	IIC_send_byte(read_c02); //写入AT24C02器件地址和读操作
	IIC_ack();
	while(len>1)
	{
		len--;
		*buf = IIC_read_byte();//读出指定地址中的数据
		IIC_Aack();
		buf++;
	}
	*buf = IIC_read_byte();//读出指定地址中的数据
	IIC_nack(); //主机发出非应答‘1’
	IIC_stop();  //停止i2c
 return dat;    //返回读出的数据
}

