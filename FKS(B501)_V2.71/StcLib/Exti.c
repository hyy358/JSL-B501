#include	"Exti.h"
#include	"Driver.h"
#include	"RfidCard.h"

/*************  �ⲿ�����ͱ������� *****************/
#ifdef EXT_INT0
/********************* INT0�жϺ��� *************************/
void Ext_INT0 (void) interrupt INT0_VECTOR		//���ж�ʱ�Ѿ������־
{
	//IrqUpSource |= 0x01;
	CardIrq();
	CardRt = 0;
}
#endif
#ifdef EXT_INT1
/********************* INT1�жϺ��� *************************/
void Ext_INT1 (void) interrupt INT1_VECTOR		//���ж�ʱ�Ѿ������־
{
//	IrqUpSource |= 0x02;
//	DtmfData = (P6 >> 4)&0x0F;
}
#endif
#ifdef EXT_INT2
/********************* INT2�жϺ��� *************************/
void Ext_INT2 (void) interrupt INT2_VECTOR	//
{

}
#endif
#ifdef EXT_INT3
/********************* INT3�жϺ��� *************************/
void Ext_INT3 (void) interrupt INT3_VECTOR
{

}
#endif
#ifdef EXT_INT4
/********************* INT4�жϺ��� *************************/
void Ext_INT4 (void) interrupt INT4_VECTOR
{

}
#endif
//========================================================================
// ����: u8	Ext_Inilize(u8 EXT, EXTI_InitTypeDef *INTx)
// ����: �ⲿ�жϳ�ʼ������.
// ����: INTx: �ṹ����,��ο�Exti.h��Ķ���.
// ����: �ɹ�����0, �ղ�������1,���󷵻�2.
// �汾: V1.0, 2012-10-22
//========================================================================
u8	Ext_Inilize(u8 EXT, EXTI_InitTypeDef *INTx)
{
//	if(EXT >  EXT_INT4)	return 1;	//�ղ���
#ifdef EXT_INT0
	if(EXT == EXT_INT0)	//���ж�0
	{
		IE0  = 0;	//���ж�0��־λ
		if(INTx->EXTI_Interrupt == ENABLE)		EX0 = 1;	//�����ж�
		else									EX0 = 0;	//��ֹ�ж�
		if(INTx->EXTI_Polity > Polity_3)	return 2;	//����
		INT0_Polity(INTx->EXTI_Polity);	//ָ���ж����ȼ�(�͵���) Polity_0,Polity_1,Polity_2,Polity_3
		if(INTx->EXTI_Mode == EXT_MODE_Fall)	IT0 = 1;	//�½����ж�
		else									IT0 = 0;	//����,�½����ж�
		return	0;		//�ɹ�
	}
#endif
#ifdef EXT_INT1
	if(EXT == EXT_INT1)	//���ж�1
	{
		IE1  = 0;	//���ж�1��־λ
		if(INTx->EXTI_Interrupt == ENABLE)		EX1 = 1;	//�����ж�
		else									EX1 = 0;	//��ֹ�ж�
		if(INTx->EXTI_Polity > Polity_3)	return 2;	//����
		INT1_Polity(INTx->EXTI_Polity);	//ָ���ж����ȼ�(�͵���) Polity_0,Polity_1,Polity_2,Polity_3
		if(INTx->EXTI_Mode == EXT_MODE_Fall)	IT1 = 1;	//�½����ж�
		else									IT1 = 0;	//����,�½����ж�
		return	0;		//�ɹ�
	}
#endif
#ifdef EXT_INT2
	if(EXT == EXT_INT2)		//���ж�2, �̶�Ϊ�½��ص����ȼ��ж�
	{
		if(INTx->EXTI_Interrupt == ENABLE)	INT_CLKO |=  (1 << 4);	//�����ж�	
		else								INT_CLKO &= ~(1 << 4);	//��ֹ�ж�
		return	0;		//�ɹ�
	}
#endif
#ifdef EXT_INT3
	if(EXT == EXT_INT3)		//���ж�3, �̶�Ϊ�½��ص����ȼ��ж�
	{
		if(INTx->EXTI_Interrupt == ENABLE)	INT_CLKO |=  (1 << 5);	//�����ж�	
		else								INT_CLKO &= ~(1 << 5);	//��ֹ�ж�
		return	0;		//�ɹ�
	}
#endif
#ifdef EXT_INT4
	if(EXT == EXT_INT4)		//���ж�4, �̶�Ϊ�½��ص����ȼ��ж�
	{
		if(INTx->EXTI_Interrupt == ENABLE)	INT_CLKO |=  (1 << 6);	//�����ж�	
		else								INT_CLKO &= ~(1 << 6);	//��ֹ�ж�
		return	0;		//�ɹ�
	}
#endif
	return 2;	//ʧ��
}
