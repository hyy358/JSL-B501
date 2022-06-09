#include	"timer.h"
#include	"RfidCard.h"
#include "Driver.h"
#include "Audio.h"

#ifdef Timer0
bit Timer0_Irq = false;
/********************* Timer0中断函数************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
   Timer0_Irq = true;
#ifdef KEY_BEEP
	KeyBeepTask();
#endif
}
#endif
#ifdef Timer1
u8 usTime;
/********************* Timer1中断函数************************/
void timer1_int (void) interrupt TIMER1_VECTOR
{
   CardRt++;
#ifdef KEY_BEEP	
	if(++usTime >= 25)
	{
		usTime = 0;
		P62 ^= KeyBeepO;
	}
#endif
}
#endif
#ifdef Timer2
/********************* Timer2中断函数************************/
void timer2_int (void) interrupt TIMER2_VECTOR
{

}
#endif
#ifdef Timer3
/********************* Timer3中断函数************************/
void timer3_int (void) interrupt TIMER3_VECTOR
{

}
#endif
#ifdef Timer4
/********************* Timer4中断函数************************/
void timer4_int (void) interrupt TIMER4_VECTOR
{
#ifdef PWM_AUDIO
	PWMtask();
#endif
}
#endif

//========================================================================
// 函数: u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
// 描述: 定时器初始化程序.
// 参数: TIMx: 结构参数,请参考timer.h里的定义.
// 返回: 成功返回0, 空操作返回1,错误返回2.
// 版本: V1.0, 2012-10-22
//========================================================================
u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
{
#ifdef Timer0
	if(TIM == Timer0)
	{
		Timer0_Stop();		//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		Timer0_InterruptEnable();	//允许中断
		else		Timer0_InterruptDisable();	//禁止中断
		if(TIMx->TIM_Polity > Polity_3)	return 2;	//错误
		Timer0_Polity(TIMx->TIM_Polity);	//指定中断优先级(低到高) Polity_0,Polity_1,Polity_2,Polity_3

		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//错误
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode;	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer0_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer0_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer0_AsCounter();	//对外计数或分频
		else		Timer0_AsTimer();	//定时
		if(TIMx->TIM_ClkOut == ENABLE)	Timer0_CLKO_Enable();	//输出时钟
		else		Timer0_CLKO_Disable();	//不输出时钟
		
		T0_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer0_Run();	//开始运行
		return	0;		//成功
	}
#endif
#ifdef Timer1
	if(TIM == Timer1)
	{
		Timer1_Stop();		//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		Timer1_InterruptEnable();	//允许中断
		else		Timer1_InterruptDisable();	//禁止中断
		if(TIMx->TIM_Polity > Polity_3)	return 2;	//错误
		Timer1_Polity(TIMx->TIM_Polity);	//指定中断优先级(低到高) Polity_0,Polity_1,Polity_2,Polity_3
		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//错误
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode;	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer1_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer1_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer1_AsCounter();	//对外计数或分频
		else		Timer1_AsTimer();	//定时
		if(TIMx->TIM_ClkOut == ENABLE)	Timer1_CLKO_Enable();	//输出时钟
		else		Timer1_CLKO_Disable();	//不输出时钟
		
		T1_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer1_Run();	//开始运行
		return	0;		//成功
	}
#endif
#ifdef Timer2
	if(TIM == Timer2)		//Timer2,固定为16位自动重装, 中断无优先级
	{
		Timer2_Stop();	//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		Timer2_InterruptEnable();	//允许中断
		else		Timer2_InterruptDisable();	//禁止中断
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer2_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer2_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer2_AsCounter();	//对外计数或分频
		else		Timer2_AsTimer();		//定时
		if(TIMx->TIM_ClkOut == ENABLE)	Timer2_CLKO_Enable();		//输出时钟
		else		Timer2_CLKO_Disable();	//不输出时钟

		T2_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer2_Run();	//开始运行
		return	0;		//成功
	}
#endif
#ifdef Timer3
	if(TIM == Timer3)		//Timer3,固定为16位自动重装, 中断无优先级
	{
		Timer3_Stop();	//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		Timer3_InterruptEnable();	//允许中断
		else		Timer3_InterruptDisable();	//禁止中断
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer3_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer3_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer3_AsCounter();	//对外计数或分频
		else		Timer3_AsTimer();		//定时
		if(TIMx->TIM_ClkOut == ENABLE)	Timer3_CLKO_Enable();		//输出时钟
		else		Timer3_CLKO_Disable();	//不输出时钟

		T3_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer3_Run();	//开始运行
		return	0;		//成功
	}
#endif
#ifdef Timer4
	if(TIM == Timer4)		//Timer3,固定为16位自动重装, 中断无优先级
	{
		Timer4_Stop();	//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		Timer4_InterruptEnable();	//允许中断
		else		Timer4_InterruptDisable();	//禁止中断
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	Timer4_12T();	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		Timer4_1T();	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	Timer4_AsCounter();	//对外计数或分频
		else		Timer4_AsTimer();		//定时
		if(TIMx->TIM_ClkOut == ENABLE)	Timer4_CLKO_Enable();		//输出时钟
		else		Timer4_CLKO_Disable();	//不输出时钟

		T4_Load(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	Timer4_Run();	//开始运行
		return	0;		//成功
	}
#endif
	return 2;	//错误
}
