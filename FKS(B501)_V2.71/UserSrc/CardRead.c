#include "Driver.h"
#include "RfidCard.h"
#include "stdio.h"

uint8_t Cnt = 0;
uint8_t CardRt = 0;
uint8_t CardGetBit = 0;
uint8_t CardBitCount = 0;
uint8_t CardChCount = 0;
uint16_t CardCh = 0;
//bool sth = false;
uint8_t RfidCardBuf[20];
uint8_t CardCode[RFID_CARD_LEN];
bool CardReadOK = false;
bool CardGetOk = false;
uint32_t CodeOld = 0;
uint8_t CodeCheckTs = 0;
uint32_t CodeIn = 0;
uint16_t CardInTime = 1000;
uint16_t CardRaw[20];

uint8_t WG[26];
uint8_t WGstr[3];

void CardReadInit(void)
{
	CardRt = 0;
	CardGetBit = 0;
	CardCh = 0;
	CardBitCount = 0;
	CardChCount = 0;
	CardReadOK = false;
	CardGetOk = false;
}

void CardMsTask(void)
{
	if(CardInTime)
	{
		if(--CardInTime == 0)
		{
			CodeOld = 0;
			CodeCheckTs = 0;
			CodeIn = 0;
		}
	}
}


bool errflg = false;

void WG_Send26(uint8_t *str)  //把26位韦根数据转换为3字节数据存储
{

	uint8_t Even = 0; //偶检验
	uint8_t Odd = 0; //奇检验
	uint8_t k;
	uint8_t CheakEven = 0;
	uint8_t CheakOdd = 1;
	for(k=0;k<26;k++)
	{
		if(k<=0)  //读偶检验位
		{
			if(str[k]==0)
					Even=0;
				else
					Even=1;
		}
		if(k>=25) //读奇检验位
			{
				if(str[k]==0)
					Odd=0;
				else
					Odd=1;
			}
			
			if(k<=8)  //读HID码低8位
			{
				if(str[k]==0x00)
					WGstr[0]|=0x00;
				else
				{
					WGstr[0]|=0x01;
					CheakEven= ~CheakEven;	 //	根据HID码低8位1的个数来确定偶检验位是1还是0
				}
				if(k<8)
				WGstr[0]=WGstr[0]<<1;
			}
			if(k<=16)  //读PID码高8位
			{
				if(str[k]==0x00)
					WGstr[1]|=0x00;
				else
				{
					WGstr[1]|=0x01;
					CheakOdd=~CheakOdd;	////	根据PID码高8位1的个数来确定奇检验位是1还是0
				}
				if(k<16)
				WGstr[1]=WGstr[1]<<1;
			}
			else		 //读PID码的低8位
			{
				if(str[k]==0x00)
					WGstr[2]|=0x00;
				else
				{
					WGstr[2]|=0x01;
					CheakOdd=~CheakOdd;	 //	根据PID码低8位1的个数来确定奇检验位是1还是0
				}
				if(k<24)
				WGstr[2]=WGstr[2]<<1;
			}
				
	}

}

//void DATA0(void)
//{
//			WG[Cnt]=0x00;
//			Cnt++;
//}

//void DATA1(void)
//{
//			WG[Cnt]=0x01;
//			Cnt++;
//}

void WGOK(void)
{
	
	WG_Send26(WG);
}

void CheckRfidCard(void)   
{
	uint8_t i,n,k;
	uint8_t j = 0;
	uint32_t num;
	uint8_t tmp;
	uint8_t chk;
	
	CardInTime = 1000;
//	printf("==================\r\n");
//	for(i=0;i<4;i++)
//	{
//		printf("%04X,\r\n",CardRaw[i]);
//	}
	i=0;
	while(((CardRaw[0]&0x8000) == 0) && (i<16))
	{
		i++;
		CardRaw[0] <<= 1;
	}
	if(i == 16)
	{
		return;
	}
	n = i;
	for(i=0;i<3;i++)
	{
		CardRaw[i] |= CardRaw[i+1]>>(16-n);
		CardRaw[i+1] <<= n;
	}
		
	for(k=0;k<3;k++)
	{
		errflg = false;

		if((k>0) && (n > 0))
		{
			for(i=3;i>0;i--)
			{
				CardRaw[i] >>= 1;
				CardRaw[i] |= ((CardRaw[i-1]<<15)&0x8000);
			}
			CardRaw[0] >>= 1;
			n--;
		}

		RfidCardBuf[0] = (CardRaw[0] >> 11)&0x1F;
		RfidCardBuf[1] = (CardRaw[0] >> 6)&0x1F;
		RfidCardBuf[2] = (CardRaw[0] >> 1)&0x1F;
		RfidCardBuf[3] = (CardRaw[0] << 4)&0x1F;
		
		RfidCardBuf[3] |= (CardRaw[1] >> 12)&0x0F;
		RfidCardBuf[4] = (CardRaw[1] >> 7)&0x1F;
		RfidCardBuf[5] = (CardRaw[1] >> 2)&0x1F;
		RfidCardBuf[6] = (CardRaw[1] << 3)&0x1F;
		
		RfidCardBuf[6] |= (CardRaw[2] >> 13)&0x0F;
		RfidCardBuf[7] = (CardRaw[2] >> 8)&0x1F;
		RfidCardBuf[8] = (CardRaw[2] >> 3)&0x1F;
		RfidCardBuf[9] = (CardRaw[2] << 2)&0x1F;
		RfidCardBuf[9] |= (CardRaw[2] >> 14)&0x0F;

		for(i=0;i<CARD_BUF_LEN;i++)
		{
			tmp = (RfidCardBuf[i]>>1)&0x0F;
			for(j=0,chk = 0;j<4;j++)
			{
				if((tmp & 0x01) == 0x01)
				{
					chk++;
				}
				tmp >>= 1;
			}
			//printf("%b02X,%bd\r\n",RfidCardBuf[i],chk);
			chk %= 2;
			//chk <<= 4;
			
			if((i%2) == 0)
			{
				CardCode[i/2] = (RfidCardBuf[i]<<3)&0xF0;
			}
			else
			{
				CardCode[i/2] |= (RfidCardBuf[i]>>1)&0x0F;
			}
				
			if(((RfidCardBuf[i]&0x01) != chk) && (i<6))//((i/2) < (CARD_BUF_LEN/2-1)))
			{
				errflg = true;
				break;
			}		
		}
		if(!errflg)
		{
			num = CardCode[0];
			num <<= 8;
			num |= CardCode[1];
			num <<= 8;
			num |= CardCode[2];
			num <<= 8;
			num |= CardCode[3];
			
			if((CodeOld == num) && (num != 0) && (num != 0xFFFFFFFF))
			{
				if(++CodeCheckTs == 1)
				{
					CodeCheckTs = 1;
					CardGetOk = true;//(CodeIn!=CodeOld);
					CodeIn = CodeOld;
					printf("CardNo:%lu\r\n",(num>>8));
					printf("CardRead:");
					for(i=0;i<j;i++)
					{
						printf("%b02X",CardCode[i]);
					}
					printf("\r\n");
					return;
				}
			}
			else
			{
				CodeOld = num;
				CodeCheckTs = 0;
			}
			return;
		}
	}
}

#define CARD_TL_MIN 17
#define CARD_TL_MAX 35
#define CARD_TH_MIN 40
#define CARD_TH_MAX 65
#define CARD_LEN 16
void CardIrq(void)
{
	
	if(CardReadOK)
	{
		return;
	}

	if((CardRt >= CARD_TL_MIN) && (CardRt <= CARD_TL_MAX))
	{
		CardGetBit++;
		if(((CardGetBit == 1) && (RFID_DAT0 == 0))
			|| ((CardGetBit == 2) && (RFID_DAT0 == 1)))
		{
			CardGetBit = 0;
			CardBitCount = 0;
			return;
		}

		if((CardGetBit > CARD_LEN) && (CardGetBit%2) == 1)//位跳变
		{
			CardCh <<= 1;
			if(RFID_DAT0 == 0)//上升沿为1
			{
				CardCh |= 0x01;
			}
			if(++CardBitCount == 16)
			{
				CardBitCount = 0;
				CardRaw[CardChCount++] = CardCh;
				if(CardChCount >= 5)
				{
					CardReadOK = true;
				}
			}
		}
	}
	else if((CardRt >= CARD_TH_MIN) && (CardRt <= CARD_TH_MAX))
	{	
		if(CardGetBit <= CARD_LEN)//未接收到完整起始位
		{
			CardGetBit = 0;
			CardBitCount = 0;
			return;
		}
		
		CardGetBit+=2;
		if((CardGetBit%2) == 1)//位跳变
		{
			CardCh <<= 1;
			if(RFID_DAT0 == 0)//上升沿为1
			{
				CardCh |= 0x01;
			}
			if(++CardBitCount == 16)
			{
				CardBitCount = 0;
				CardRaw[CardChCount++] = CardCh;
				if(CardChCount >= 5)
				{
					CardReadOK = true;
				}
			}
		}
	}
	else//位错误
	{
		CardGetBit = 0;
		CardBitCount = 0;
	}
}
