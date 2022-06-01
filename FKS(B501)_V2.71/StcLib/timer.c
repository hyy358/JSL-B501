#include "timer.h"
#include "RfidCard.h"
#include "Driver.h"
#include "Audio.h"

#ifdef Timer0
bit Timer0_Irq = false;
u8 usTime;
/********************* Timer0�жϺ���************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	Timer0_Irq = true;

#ifdef PWM_BEEP
	PWMtask();
#endif
#ifdef KEY_BEEP
	KeyBeepTask();
#endif
#ifdef PWM_RING
	RingTask();
#endif
}
#endif
#ifdef Timer1
/********************* Timer1�жϺ���************************/
void timer1_int (void) interrupt TIMER1_VECTOR
{
   CardRt++;

	if(++usTime >= 25)
	{
		usTime = 0;
#ifdef KEY_BEEP	
		P60 ^= KeyBeepO;
#endif

	}

}
#endif
#ifdef Timer2
/********************* Timer2�жϺ���************************/
void timer2_int (void) interrupt TIMER2_VECTOR
{

}
#endif
#ifdef Timer3
/********************* Timer3�жϺ���************************/
void timer3_int (void) interrupt TIMER3_VECTOR
{

}
#endif
#ifdef Timer4
u8 t4u = 0;
/********************* Timer4�жϺ���************************/
void timer4_int (void) interrupt TIMER4_VECTOR
{
#ifdef PWM_AUDIO
	PWMtask();
#endif
#ifdef PWM_RING
	if((RingO > 0) && (++t4u >= RingO))
	{
		t4u = 0;
		P63 ^= 1;
	}
#endif
}
#endif

//========================================================================
// ����: u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
// ����: ��ʱ����ʼ������.
// ����: TIMx: �ṹ����,��ο�timer.h��Ķ���.
// ����: �ɹ�����0, �ղ�������1,���󷵻�2.
// �汾: V1.0, 2012-10-22
//========================================================================
u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
{
#ifdef Timer0
	if(TIM == Timer0)
	{
		Timer0_Stop();		//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		Timer0_InterruptEnable();	//�����ж�
		else		Timer0_InterruptDisable();	//��ֹ�ж�
		if(TIMx->TIM_Polity > Polity_3)	return 2;	//����
		Timer0_Polity(TIMx->TIM_Polity);	//ָ���ж����ȼ�(�͵���) Polity_0,Polity_1,Polity_2,Polity_3

		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//����
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode;	//����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer0_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer0_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer0_AsCounter();	//����������Ƶ
		else		Timer0_AsTimer();	//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	Timer0_CLKO_Enable();	//���ʱ��
		else		Timer0_CLKO_Disable();	//�����ʱ��
		
		T0_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer0_Run();	//��ʼ����
		return	0;		//�ɹ�
	}
#endif
#ifdef Timer1
	if(TIM == Timer1)
	{
		Timer1_Stop();		//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		Timer1_InterruptEnable();	//�����ж�
		else		Timer1_InterruptDisable();	//��ֹ�ж�
		if(TIMx->TIM_Polity > Polity_3)	return 2;	//����
		Timer1_Polity(TIMx->TIM_Polity);	//ָ���ж����ȼ�(�͵���) Polity_0,Polity_1,Polity_2,Polity_3
		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//����
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode;	//����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer1_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer1_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer1_AsCounter();	//����������Ƶ
		else		Timer1_AsTimer();	//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	Timer1_CLKO_Enable();	//���ʱ��
		else		Timer1_CLKO_Disable();	//�����ʱ��
		
		T1_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer1_Run();	//��ʼ����
		return	0;		//�ɹ�
	}
#endif
#ifdef Timer2
	if(TIM == Timer2)		//Timer2,�̶�Ϊ16λ�Զ���װ, �ж������ȼ�
	{
		Timer2_Stop();	//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		Timer2_InterruptEnable();	//�����ж�
		else		Timer2_InterruptDisable();	//��ֹ�ж�
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer2_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer2_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer2_AsCounter();	//����������Ƶ
		else		Timer2_AsTimer();		//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	Timer2_CLKO_Enable();		//���ʱ��
		else		Timer2_CLKO_Disable();	//�����ʱ��

		T2_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer2_Run();	//��ʼ����
		return	0;		//�ɹ�
	}
#endif
#ifdef Timer3
	if(TIM == Timer3)		//Timer3,�̶�Ϊ16λ�Զ���װ, �ж������ȼ�
	{
		Timer3_Stop();	//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		Timer3_InterruptEnable();	//�����ж�
		else		Timer3_InterruptDisable();	//��ֹ�ж�
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer3_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer3_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer3_AsCounter();	//����������Ƶ
		else		Timer3_AsTimer();		//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	Timer3_CLKO_Enable();		//���ʱ��
		else		Timer3_CLKO_Disable();	//�����ʱ��

		T3_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer3_Run();	//��ʼ����
		return	0;		//�ɹ�
	}
#endif
#ifdef Timer4
	if(TIM == Timer4)		//Timer3,�̶�Ϊ16λ�Զ���װ, �ж������ȼ�
	{
		Timer4_Stop();	//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		Timer4_InterruptEnable();	//�����ж�
		else		Timer4_InterruptDisable();	//��ֹ�ж�
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer4_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer4_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer4_AsCounter();	//����������Ƶ
		else		Timer4_AsTimer();		//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	Timer4_CLKO_Enable();		//���ʱ��
		else		Timer4_CLKO_Disable();	//�����ʱ��

		T4_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer4_Run();	//��ʼ����
		return	0;		//�ɹ�
	}
#endif
	return 2;	//����
}
