#include	"Exti.h"
#include	"Driver.h"
#include	"RfidCard.h"

/*************  外部函数和变量声明 *****************/
#ifdef EXT_INT0
/********************* INT0中断函数 *************************/
void Ext_INT0 (void) interrupt INT0_VECTOR		//进中断时已经清除标志
{
	//IrqUpSource |= 0x01;
	CardIrq();
	CardRt = 0;
}
#endif
#ifdef EXT_INT1
/********************* INT1中断函数 *************************/
void Ext_INT1 (void) interrupt INT1_VECTOR		//进中断时已经清除标志
{
//	IrqUpSource |= 0x02;
//	DtmfData = (P6 >> 4)&0x0F;
}
#endif
#ifdef EXT_INT2
/********************* INT2中断函数 *************************/
void Ext_INT2 (void) interrupt INT2_VECTOR	//
{

}
#endif
#ifdef EXT_INT3
/********************* INT3中断函数 *************************/
void Ext_INT3 (void) interrupt INT3_VECTOR
{

}
#endif
#ifdef EXT_INT4
/********************* INT4中断函数 *************************/
void Ext_INT4 (void) interrupt INT4_VECTOR
{

}
#endif
//========================================================================
// 函数: u8	Ext_Inilize(u8 EXT, EXTI_InitTypeDef *INTx)
// 描述: 外部中断初始化程序.
// 参数: INTx: 结构参数,请参考Exti.h里的定义.
// 返回: 成功返回0, 空操作返回1,错误返回2.
// 版本: V1.0, 2012-10-22
//========================================================================
u8	Ext_Inilize(u8 EXT, EXTI_InitTypeDef *INTx)
{
//	if(EXT >  EXT_INT4)	return 1;	//空操作
#ifdef EXT_INT0
	if(EXT == EXT_INT0)	//外中断0
	{
		IE0  = 0;	//外中断0标志位
		if(INTx->EXTI_Interrupt == ENABLE)		EX0 = 1;	//允许中断
		else									EX0 = 0;	//禁止中断
		if(INTx->EXTI_Polity > Polity_3)	return 2;	//错误
		INT0_Polity(INTx->EXTI_Polity);	//指定中断优先级(低到高) Polity_0,Polity_1,Polity_2,Polity_3
		if(INTx->EXTI_Mode == EXT_MODE_Fall)	IT0 = 1;	//下降沿中断
		else									IT0 = 0;	//上升,下降沿中断
		return	0;		//成功
	}
#endif
#ifdef EXT_INT1
	if(EXT == EXT_INT1)	//外中断1
	{
		IE1  = 0;	//外中断1标志位
		if(INTx->EXTI_Interrupt == ENABLE)		EX1 = 1;	//允许中断
		else									EX1 = 0;	//禁止中断
		if(INTx->EXTI_Polity > Polity_3)	return 2;	//错误
		INT1_Polity(INTx->EXTI_Polity);	//指定中断优先级(低到高) Polity_0,Polity_1,Polity_2,Polity_3
		if(INTx->EXTI_Mode == EXT_MODE_Fall)	IT1 = 1;	//下降沿中断
		else									IT1 = 0;	//上升,下降沿中断
		return	0;		//成功
	}
#endif
#ifdef EXT_INT2
	if(EXT == EXT_INT2)		//外中断2, 固定为下降沿低优先级中断
	{
		if(INTx->EXTI_Interrupt == ENABLE)	INT_CLKO |=  (1 << 4);	//允许中断	
		else								INT_CLKO &= ~(1 << 4);	//禁止中断
		return	0;		//成功
	}
#endif
#ifdef EXT_INT3
	if(EXT == EXT_INT3)		//外中断3, 固定为下降沿低优先级中断
	{
		if(INTx->EXTI_Interrupt == ENABLE)	INT_CLKO |=  (1 << 5);	//允许中断	
		else								INT_CLKO &= ~(1 << 5);	//禁止中断
		return	0;		//成功
	}
#endif
#ifdef EXT_INT4
	if(EXT == EXT_INT4)		//外中断4, 固定为下降沿低优先级中断
	{
		if(INTx->EXTI_Interrupt == ENABLE)	INT_CLKO |=  (1 << 6);	//允许中断	
		else								INT_CLKO &= ~(1 << 6);	//禁止中断
		return	0;		//成功
	}
#endif
	return 2;	//失败
}
