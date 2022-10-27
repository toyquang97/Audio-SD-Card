
#define	_SUBPROG_C_
#include	"AllHeader.h"

//Tosc=16MHz,无中断情况下,延迟tick个毫秒
void delay_ms(unsigned int tick){
	volatile unsigned int i;

	while(tick--)
		{
			for (i = 0; i < 165; i++);
			ClrWdt();
		}
}

/* ==============   工作指示灯的控制   ==================
  亮100ms,灭100ms,再亮100ms,最后灭1s,为一个循环  
=======================================================*/  
void LedWork(void){
	static BYTE mLedCycle_Cnt = 0;
	//nop();
	mLedCycle_Cnt++;
	if(mLedCycle_Cnt < 4)
		{
			if(mLedCycle_Cnt % 2)
				OPEN_RUN_LED();
			else
				CLOSE_RUN_LED();
		}
	else if(mLedCycle_Cnt >= 4 && mLedCycle_Cnt < 15)
		CLOSE_RUN_LED();
	else if(mLedCycle_Cnt >= 15)
		mLedCycle_Cnt = 0;
}

/**********************************************************************************
 - 功能描述： 播报功能选择(读取 SW2 的4位拨码以及音量调节拨码)
 - 隶属模块： 外部
 - 参数说明： 无
 - 返回说明： 无
 - 注：	      
**********************************************************************************/
void Function_Config(void){
	uchar i = 0, j = 0, a = 0;	
	
	uchar k[8], m[8];
	uchar i;
	for(i = 0;i < 8;i++)
		{
			k[i] = ((PORTA & BIT_3) >> 3) | ((PORTA & BIT_2) >> 1) | ((PORTA & BIT_1) << 1) | ((PORTA & BIT_0) << 3);
			k[i] &= 0x0F;
			m[i] = PORTE & 0x03;

			Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
			Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
		}
	for(i = 0; i < 7; i++)
		{
			if(k[i] == k[i + 1])
				j++;
			if(m[i] == m[i + 1])
				a++;
		}
	if(j >= 6)
		{
			p.music_language = ~k[0] & 0x03;
			p.music_back_type = (~k[0] >> 2) & 0x03;
		}	
	if(a >= 6)
		mValumn_Switch = m[0] & 0x03;
	else
		mValumn_Switch = 1;
} 

