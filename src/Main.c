
#define	MAIN_C

//#define	_DEBUG
#include "AllHeader.h"

#pragma config FOSC = HS2			// Main oscillator high speed resonator
#pragma config XINST = OFF
#pragma config MCLRE = ON

#ifdef _DEBUG
#pragma config WDTEN = OFF		// Watchdog timer off
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF
#pragma config CPB = OFF
#else
#pragma config PWRTEN = ON
#pragma config WDTEN = SWDTDIS//ON			// Watchdog timer off
#pragma config CP0 = ON
#pragma config CP1 = ON
#pragma config CP2 = ON
#pragma config CP3 = ON
#pragma config CPB = ON
#endif
#pragma config BOREN = ON		// Brown out reset enabled
#pragma config BORV = 1			// Brown out voltage 2.7V
#pragma config WDTPS = 1024//1024//256		// watchdog postscaler 1 : 256
//#pragma config LVP = OFF		// Low voltage programming off
#pragma config CANMX = PORTB
#pragma config SOSCSEL = DIG

void Device_init(void){
  IOCB 	= 0;		// pb口，电平变化中断禁止
	WPUB 	= 0;		// pb口，弱上拉功能
	ODCON 	= 0;		// spi,uart等漏极开路控制
  HLVDCON	= 0;   		// 高低压检测	
	SLRCON	= 0;		// 压摆控制寄存器
  ANCON0 = 0x00;	// port A and E pins as digital I/O
	ANCON1 = 0x00;
    
	PORTA  = 0x00;
	LATA   = 0x00;	
	TRISA  = 0xFF;//RA0--RA3 SW1 拨码开关输入
		// RA0: SW2.4 input
		// RA1: SW2.3 input
		// RA2: SW2.2 input
		// RA3: SW2.1 input
		// RA4: Not used in
		// RA5: Not used in
		// RA6-RA7: OSC
		
	PORTB = 0xFF; 		// switch off (important for CAN!)
	LATB	= 0x00;
	TRISB = 0xFB; 
		// RB0-RB1: Not used input
		// RB2-RB3: CAN
		// RB4-RB5; Not used input
		// RB6-RB7: PGC-PGD;
    
  TRISC = 0xBF;	
		//RC0: MP3 Busy check  input
		//RC1: DI1      input
		//RC2: DI2      input
		//RC3: DI3      input
		//RC4: DI4      input
		//RC5: Not Used input
		//RC6: TX			 output
		//RC7; RX			 input
	PORTC = 0x00;
	LATC	= 0x00;
	
	PORTD = 0x00;
	LATD  = 0x00;	
	TRISD = 0xF0;
		//RD0: LED-RUN			output
		//RD1: 右声道使能		output
		//RD2: 左声道使能		output
		//RD3: 给语音芯片供电		output
		//RD4-RD7: Not used input	
	LATDbits.LATD1 = 1;		//左右声道输出禁止
	LATDbits.LATD2 = 1;
	MUSIC_POWER_OUTPUT();

	PORTE = 0x00;
	TRISE = 0x07;
		// RE0-RE2: Not used input
	LATE  = 0x00;
}

/**********************************************************************************
 - 功能描述： 定时器0初始化
 - 隶属模块： 外部
 - 参数说明： 无
 - 返回说明： 无
 - 注：	      
**********************************************************************************/
void Timer0_init(void){	
	INTCONbits.T0IF  = 0;		//清除TMR0中断标志
	INTCONbits.T0IE  = 1;		//TMR0中断允许位
	INTCON2bits.T0IP = 0;		//设置Timer0中断为低优先级
	TMR0L = T0L_500ms;				//定时器初值
	TMR0H = T0H_500ms;				//定时器初值
	T0CON = T0START_1TO32;
}

void Timer1_init (void){
	PIR1bits.TMR1IF = 0;
	IPR1bits.TMR1IP = 0;
	PIE1bits.TMR1IE = 1;
	TMR1H = T1H_5ms;
	TMR1L = T1L_5ms;
	T1CON = T1START_1TO1;
}

void Load_DefaultPara(void){
	
	if(mValumn_Switch == 1)
		p.music_volumn = 25;
	else		
		p.music_volumn = 15;
	p.music_timer = 60;
	p.floor_enable = ENABLE;
	p.arrow_enable = ENABLE;
	p.door_enable = ENABLE;
	p.input_enable = ENABLE;
	p.special_enable = DISABLE;
}


/****************************************************************************************************/
/* sET ALL USER VARIABLES																			*/
/****************************************************************************************************/
void Init_UserVar(void){
	BYTE i;
	
	for (i = 0; i < MAX_IO_TYPE; i++)
		virt_out[i] = 0;
	Function_Config();
	Load_DefaultPara();
	if ( p.music_language > MAX_LANGUAGE)
		p.music_language = ENGLISH;
	if(p.music_back_type > (MAX_PLAY_MODE - 1))
		p.music_back_type = PLAY_ALL;
}

/****************************************************************************************************/
/* main function																					*/
/****************************************************************************************************/
void main(void){	
	BYTE baudrate = 0;
	
	delay_ms(1000);
	Device_init();	
	
#ifndef _DEBUG
	WDTCON = 0x01;		//使能软件看门狗
#endif

	INTCON = 0;
	RCONbits.IPEN = 1;
	ClrWdt();	
	Timer0_init();	
	Timer1_init();	
	UART_Init(BAUD_9600);
	Init_UserVar();		

	nmtstate = BOOT_UP;
	nmtwait  = 1000;				/* waiting time to 1. heartbeat			*/
	speech_lift = LIFT1;						/* read lift for speech from EEPROM		*/	

	INTCONbits.GIEH = 1;
	INTCONbits.GIEL = 1;	
	while (nmtwait)
		ClrWdt();
	delay_ms(1000);		
	MP3_init();
	music_init_finish = 1;	
	baudrate = BPS_125K;
	Can_init (baudrate);								/* initialize Onchip CAN		l		*/
	nmtstate = OPERATIONAL;	
	
	ClrWdt();												/* reset watchdog timer					*/
	music_back_timer = p.music_timer;						/* start music timer					*/
	while(1) 
		{		
			ClrWdt();
			if(bTime_Fg.time_100ms)
				{
					if(music_led_no_good == 0)
						LedWork();										/* handle LED							*/
					Play_Input();									/* handle digital inputs				*/
					bTime_Fg.time_100ms = 0;
				}			
			if(bTime_Fg.time_500ms)
				{
					UART_ReSend();									/* handle UART errors					*/	
					UART_ResetCheck();
					bTime_Fg.time_500ms = 0;
				}
			if(uart1_status_ok)
				{
					UART_Handle(sReceive_status);					/* check UART feedback					*/
					uart1_status_ok = 0;
				}
			if (rc) 											/* Message in receive buffer			*/
				read_rx (); 									/* read and handle CAN message			*/
			if (p.music_back_type == PLAY_DEMO)						/* enter test mode						*/
				MP3_Demo ();
			else if(nmtstate == OPERATIONAL)
				{
					if(!play_timer)
						Play_Task();
					if(p.music_back_type == PLAY_BACKMUSIC_TIMER)
						{
							if ((music_back_timer || (!p.music_timer)) && (!music_back_status)) 
								{// no time out, but music is off
									UART_SendCommand(CMD_PLAY_FILE_LOOP , FEED_BACK , ((p.music_language % (MAX_LANGUAGE + 1)) + 1));
									delay_ms(500);
									music_back_status = 1;
								}
							else if ((!music_back_timer) && p.music_timer && music_back_status)
								{//time out, but music is on	
									UART_SendCommand(CMD_PLAY_FILE_LOOP , FEED_BACK , MUSIC_BACK_FILE_NO);
									delay_ms(500);
									music_back_status = 0;
								}				
							if(updown_msg & 0x33)
								{
									music_back_timer = p.music_timer;
								}
						}
					else if (p.music_back_type == PLAY_ALL)
						{
							if (!music_back_status)					/* music is off							*/	
								{
									UART_SendCommand(CMD_PLAY_FILE_LOOP , FEED_BACK , ((p.music_language % (MAX_LANGUAGE + 1)) + 1));
									delay_ms(500);
									music_back_status = 1;
								}
						}
					else if (p.music_back_type == PLAY_NO_BACKMUSIC)
						{
							if (music_back_status)						/* music is on							*/	
								{
									UART_SendCommand(CMD_PLAY_FILE_LOOP , FEED_BACK , MUSIC_BACK_FILE_NO);
									delay_ms(500);
									music_back_status = 0;
								}
						}
				}
			if((CanErr_Flag && (!can_inittime)) || (com_can_work > 20))
				{
					CanErr_Flag = 0;
					com_can_work = 0;
					Can_init(baudrate);
				}
			if(!PlayOverFlag)
				{
					LATDbits.LATD1 = 0;
					LATDbits.LATD2 = 0;
				}
		}	
}



