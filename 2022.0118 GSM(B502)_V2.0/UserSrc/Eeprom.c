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
void IIC_start() //����i2c
{
	IIC_DAT_H;
	IIC_delay();
	IIC_CLK_H;
	IIC_delay();
	IIC_DAT_L;
	IIC_delay();
}

void IIC_stop() //ֹͣi2c
{
	IIC_DAT_L;
	IIC_delay();
	IIC_CLK_H;
	IIC_delay();
	IIC_DAT_H; //��sclΪ�ߵ�ƽʱ��sdasdaһ��������Ϊֹͣ�ź�
	IIC_delay();
}
//ack:
void IIC_ack() //Ӧ���ź�0
{
	uint8_t i = 0; //�ȴ�����
	IIC_CLK_H;
	IIC_delay();
	while((IIC_DAT == 1) && i < 250)i++;//�ȴ�һ��ʱ��//��ΪӦ��0���˳����ӻ�����������Ӧ���ź�
	IIC_CLK_L; //Ӧ��֮��scl����
	IIC_delay();
}
//nack:
void IIC_nack() //��Ӧ��
{
	IIC_DAT_H;
	IIC_CLK_L;//��sclΪ�ߵ�ƽ�ڼ䣬��������ӻ�����һ��1����Ӧ���ź�
	IIC_delay();
	IIC_CLK_H; //Ӧ��֮��scl����
	IIC_delay();
	IIC_CLK_L;
	IIC_DAT_L;
}

void IIC_Aack() //Ӧ��
{
	IIC_DAT_L;
	IIC_CLK_L;//��sclΪ�ߵ�ƽ�ڼ䣬��������ӻ�����һ��1����Ӧ���ź�
	IIC_delay();
	IIC_CLK_H; //Ӧ��֮��scl����
	IIC_delay();
	IIC_CLK_L;
}


//send byte:
void IIC_send_byte(uint8_t date)//дһ���ֽ�
{
	uint8_t i, temp;
	temp = date; //����Ҫд������ݣ���Ҫ���͵�sda�ϵ�����
	for(i = 0; i < 8; i++)
	{ //����8λ
		IIC_CLK_L;
		IIC_delay();
		IIC_DAT = ((temp&0x80)==0x80)?1:0;
		temp <<= 1;
		IIC_delay();
		IIC_CLK_H; //��sclΪ�ߵ�ƽʱ��������sda�ϵ����ݱ仯��ʹ�����ȶ�
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
uint8_t IIC_read_byte() //��һ���ֽ�����
{
	uint8_t i,k;
	IIC_CLK_L;
	IIC_delay();
	IIC_DAT_H; //����sda�仯
	IIC_delay(); //ʹsda�����ȶ���ʼ������
	for(i = 0; i < 8; i++)
	{
		IIC_CLK_H;
		IIC_delay();
		k <<= 1;
		k |= IIC_DAT;
		IIC_CLK_L;
		IIC_delay();
	}
	return k;//���ض���������
}
//write:at24c02  ��at24c02�е�ָ����ַд������
//void write_at24c02(u16 address, u8 date)
//{
//	IIC_WP_L;
//	IIC_delay();
//	IIC_start(); //����i2c
//	IIC_send_byte(write_c02);//д���ڼ��ַ��д����
//	IIC_ack(); //�ӻ�Ӧ��0
//	IIC_send_byte((address>>8)&0xFF); //д��д���ݵĵ�Ԫ��ַ
//	IIC_ack();
//	IIC_send_byte(address&0xFF); //д��д���ݵĵ�Ԫ��ַ
//	IIC_ack(); //ack0
//	IIC_send_byte(date); //��ָ����ַ��д������
//	IIC_ack(); //�ӻ�Ӧ��0
//	IIC_stop();    //ֹͣi2c
//	//IIC_Wp_H;
//}
////read: at24c02   ��at24c02��ָ����ַ�ж���д�������
//u8 read_at24c02(u16 address)
//{
//	u8 dat;//�����洢����������
//	IIC_start(); //����i2c
//	IIC_send_byte(write_c02); //д��at24c02������ַ��д����
//	IIC_ack(); //�ӻ�Ӧ��0
//	IIC_send_byte((address>>8)&0xFF);
//	IIC_ack(); 
//	IIC_send_byte(address&0xFF);
//	IIC_ack(); //�ӻ�Ӧ��0
//	IIC_start();  //�ٴ�����i2c
//	IIC_send_byte(read_c02); //д��AT24C02������ַ�Ͷ�����
//	IIC_ack();//�ӻ�Ӧ��0��
//	dat = IIC_read_byte();//����ָ����ַ�е�����
//	//IIC_nack(); //����������Ӧ��1��
//	IIC_stop();  //ֹͣi2c
// return dat;    //���ض���������
//}

void writeBuf_at24c02(uint16_t address, uint8_t *buf,uint16_t len)
{
	IIC_WP_L;
	IIC_start(); //����i2c
	IIC_send_byte(write_c02);//д���ڼ��ַ��д����
	IIC_ack();
	IIC_send_byte((address>>8)&0xFF);
	IIC_ack();
	IIC_send_byte(address&0xFF);
	IIC_ack();
	while(len--)
	{
		IIC_send_byte(*buf++); //��ָ����ַ��д������
		IIC_ack();
		address++;
		if(((address%EEPROM_PAGESIZE) == 0) && (len > 0))
		{
			IIC_stop();
			IIC_delayms(100);
			IIC_start(); //����i2c
			IIC_send_byte(write_c02);//д���ڼ��ַ��д����
			IIC_ack();
			IIC_send_byte((address>>8)&0xFF);
			IIC_ack();
			IIC_send_byte(address&0xFF);
			IIC_ack();
		}
	}
	IIC_stop();    //ֹͣi2c
	IIC_delayms(100);
}

uint8_t readBuf_at24c02(uint16_t address, uint8_t *buf,uint16_t len)
{
	uint8_t dat;//�����洢����������
	IIC_start(); //����i2c
	IIC_send_byte(write_c02); //д��at24c02������ַ��д����
	IIC_ack();
	IIC_send_byte((address>>8)&0xFF);
	IIC_ack();
	IIC_send_byte(address&0xFF);
	IIC_ack();
	IIC_start();  //�ٴ�����i2c
	IIC_send_byte(read_c02); //д��AT24C02������ַ�Ͷ�����
	IIC_ack();
	while(len>1)
	{
		len--;
		*buf = IIC_read_byte();//����ָ����ַ�е�����
		IIC_Aack();
		buf++;
	}
	*buf = IIC_read_byte();//����ָ����ַ�е�����
	IIC_nack(); //����������Ӧ��1��
	IIC_stop();  //ֹͣi2c
 return dat;    //���ض���������
}

