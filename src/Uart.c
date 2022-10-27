
#define	_UART_C_
#include "AllHeader.h"	

/**********************************************************************************
 - 功能描述： 串口1初始化
 - 隶属模块： 外部
 - 参数说明： 无
 - 返回说明： 无
 - 注：	      都是9600波特率
**********************************************************************************/
void UART_Init(uint baud){	
	TRISCbits.TRISC6 = 0; 	//异步方式
	TRISCbits.TRISC7 = 1;
	
	BAUDCON1bits.BRG16 = 0;		//8位模式
	SPBRG1 = baud;
	SPBRGH1 = baud >> 8;	
	SPBRGH1 = 0;
	SPBRGH1 = 0;

	TXSTA1 = 0x20;
	RCSTA1 = 0x90;		//使能接收器
	RCSTA1 = 0x90;
	BAUDCON1bits.TXCKP = 0;
	
	PIR1bits.TX1IF = 0;
	IPR1bits.TX1IP = 0; 	//串口发送为低优先级
	PIE1bits.TX1IE = 0;
	IPR1bits.RC1IP = 0; 	//串口接收为低优先级
	PIE1bits.RC1IE = 1;
}

/**********************************************************************************
 - 功能描述： 串口发送一个字节
 - 隶属模块： 外部
 - 参数说明：
 - 返回说明：
 - 注：	      
***********************************************************************************/
void UART_SendOneByte(uchar byte){
	TXREG1 = byte;
	while(! TXSTA1bits.TRMT);	 
}

/**********************************************************************************
 - 功能描述： 串口发送一串字节
 - 隶属模块： 外部
 - 参数说明：
 - 返回说明：
 - 注：	      
***********************************************************************************/
void UART_SendBuf(BYTE* pBuf, BYTE len){
	BYTE i;
	
	for(i=0; i<len; i++)
		{
			UartBuf.TxBuf[uart1_send_to++] = pBuf[i];
			if (uart1_send_to == USART_TX_BUFFER_SIZE)
				uart1_send_to = 0;
		}
	INTCONbits.GIEH	= 0;
	uart1_send_len += len;
	if (!uart1_send_busy)
		{
			TXREG1 = UartBuf.TxBuf[uart1_send_ro++];
			if (uart1_send_ro == USART_TX_BUFFER_SIZE)
				uart1_send_ro = 0;
			-- uart1_send_len;
			uart1_send_busy = 1;
		}
	PIE1bits.TX1IE	= 1;
	INTCONbits.GIEH	= 1;
}

/**********************************************************************************
 - 功能描述： 串口数据重发处理函数
 - 隶属模块： 外部
 - 参数说明：
 - 返回说明：
 - 注：
**********************************************************************************/
void UART_ReSend(void){
	if(Resend_Flag)
		{//如果 Resend_Flag = 1 时间溢出还没有收到ACK 则重发
			ResendCount ++;
			if(ResendCount > RESEND_MAX_CNT)
				{
					ResendCount  = 0;
					Resend_Flag  = 0;
					return;
				}
			UART_SendBuf(Send_buf, sizeof(Send_buf));
			UartRecvACK = WAIT_ACK_500MS ;//设定等待应答的时间[300ms] 
		}
}

/**********************************************************************************
 - 功能描述： 检查play管脚是否忙
 - 隶属模块： 外部
 - 参数说明：
 - 返回说明：
 - 注：
**********************************************************************************/
uchar UART_BusyCheck(void){	
	uchar busy_value = 0;
	
	busy_value = PORTC & 0x01;
	if(busy_value)
 		return NOT_BUSY;
 	else
 		return PALY_BUSY;  
}

/********************************************************************************************
 - 功能描述：求和校验
 - 隶属模块：
 - 参数说明：
 - 返回说明：
 - 注：      和校验的思路如下
             发送的指令，去掉起始和结束。将中间的6个字节进行累加，最后取反码
             接收端就将接收到的一帧数据，去掉起始和结束。将中间的数据累加，再加上接收到的校验
             字节。刚好为0.这样就代表接收到的数据完全正确。
********************************************************************************************/
void UART_CheckSum( uchar *Str, uchar len){
	uint xorsum = 0;
	uchar i;

	for(i = 0; i < len; i++)
	  xorsum += Str[i];
	xorsum  = 0 - xorsum;
	Str[i] = (uchar)(xorsum >> 8);
	Str[i + 1] = (uchar)xorsum;
}

/********************************************************************************************
 - 功能描述： 串口向外发送命令[包括控制和查询]
 - 隶属模块： 外部
 - 参数说明： CMD:表示控制指令，请查阅指令表，还包括查询的相关指令
              feedback:是否需要应答[0:不需要应答，1:需要应答]
              data:传送的参数
 - 返回说明：
 - 注：       
********************************************************************************************/
void UART_SendCommand(uchar CMD, uchar feedback, uint dat){
	Send_buf[UART_RECV_IDLE] = MP3_SEND_STARTBYTE_1;    	//保留字节 
	Send_buf[UART_RECV_VER] = MP3_SEND_STARTBYTE_2;    	//保留字节 
  Send_buf[UART_RECV_LENTH] = 0x06;    								//长度
  Send_buf[UART_RECV_CMD] = CMD;     									//控制指令
  Send_buf[UART_RECV_FEEDBACK] = feedback;							//是否需要反馈
  Send_buf[UART_RECV_DATAH] = (uchar)(dat >> 8);				//datah
  Send_buf[UART_RECV_DATAL] = (uchar)dat;     				//datal
  Send_buf[UART_RECV_OVER] = MP3_SEND_STOPBYTE;
  UART_CheckSum(&Send_buf[UART_RECV_VER], 6);   		   	//校验
  UART_SendBuf(Send_buf, sizeof(Send_buf));
  UartRecvACK = WAIT_ACK_500MS ;										//设定等待应答的时间[300ms] 
}

/***************************************************************************
 - 功能描述： 串口任务处理函数
 - 隶属模块： 内部
 - 参数说明： *pr ：串口0需要接收的数据的地址指针
 - 返回说明： 无
 - 注：	
***************************************************************************/
void UART_Handle(uchar *pBuf){
  WORD checksum = 0;
	BYTE command, i;
	BYTE data_h, data_l;

	for(i = 1; i < 7; i++)
		checksum += pBuf[i];
	checksum += (((WORD)pBuf[7] << 8) | pBuf[8]);
	if(!checksum)
		{//校验码OK
			UartRecvACK = 0 ;
			Resend_Flag  = 0;
			ResendCount  = 0; 						
			command = pBuf[UART_RECV_CMD];
			data_h = pBuf[UART_RECV_DATAH];
			data_l = pBuf[UART_RECV_DATAL];
		  switch(command)
			  {			  	
		      case (UARTQUR_START_UP):				//模块上电返回的数据
		        OnlineDevice = data_l ;
						PlayDevice = data_l ;
		        break ;

		      case (UARTQUR_CARD_INSERT):					//设备插入
		        if(PlayDevice == data_l)
							OnlineDevice = PlayDevice ;		//在线设备已经去除
		        break;
						
		      case (UARTQUR_CARD_OUTPUT):					//设备拔出
		        if(PlayDevice == data_l)
							OnlineDevice = PLAYDEVICE_NULL ;						
		        break;
		                  
		      case (UARTQUR_STAY_TF):					//TF当前曲目播放完毕
						play_status = 0;
		        break ;
						
		      case (UARTQUR_ERR):				//模块返回的错误
			      uart1_err_status = data_l;
						if(data_l == 0x01)
							{//忙标志
								//do nothing
							}
						else if((data_l == 0x02)	||	//处于睡眠模式
								(data_l == 0x05)	||			//文件超出范围
								(data_l == 0x06))					//找不到指定文件
							{
								reset_flag = 1;		//重新对串口进行初始化
							}
						else if((data_l == 0x03)	||	//一帧数据没有接受完成
								(data_l == 0x04))					//校验和出错,重新发送
							{
								Resend_Flag = 1;		//设置重发标志
							}
						else
							{//do nothing
							}
		        break ;  
						
					case (UARTQUR_ACK):					//收到应答ACK
						UartRecvACK = 0 ;
						Resend_Flag  = 0;
						ResendCount  = 0; 						
						break ;
						
		      default:	break;
		  	}
	  }
}

void UART_ResetCheck(void){
	if(reset_flag)
		{
			reset_flag = 0;
			music_output_timer = 0;
			INTCONbits.GIEH = 0;
			UART_SendCommand(STOP_CMD , FEED_BACK , 0);
			delay_ms(500);				//复位等待的时间不可以太短，至少需要300ms以上
//增加冗余处理	
			UART_Init(BAUD_9600);		//重新初始化UART以及MP3
			delay_ms(500);
			MP3_init();
			INTCONbits.GIEH = 1;
			delay_ms(1000);
		}
}

/****************************************************************************************************/
/* Init MP3 chip																					*/
/****************************************************************************************************/
void MP3_init(void){
	music_volumn = p.music_volumn;
	UART_SendCommand(CMD_VOLUMN_SETTING , FEED_BACK , music_volumn);	
	delay_ms(500);									/* set volume									*/
	music_back_status = 0;
	floor_flag = 0;
	updown_flag = 0;
	door_flag = 0;
	warn_flag = 0;
//初始化时不可缺少播放的指令
	if(p.music_back_type != PLAY_NO_BACKMUSIC) 
		UART_SendCommand(CMD_PLAY_FILE_LOOP , FEED_BACK , ((p.music_language % (MAX_LANGUAGE + 1)) + 1));//指定文件夹01--04循环播放
	else 
		UART_SendCommand(CMD_PLAY_FILE_LOOP , FEED_BACK , MUSIC_BACK_FILE_NO);//指定静音文件夹05循环播放
	delay_ms(1000);	
	LATDbits.LATD1 = 0;
	LATDbits.LATD2 = 0;
}

/**********************************************************************************
 - 功能描述： 语音播放任务函数
 - 隶属模块： 外部
 - 参数说明： 无
 - 返回说明： 无
 - 注：	      
**********************************************************************************/
void Play_Task(void) {
	WORD files = 0;
	BYTE i = 0, j = 0;
	
	if(floor_flag)
		{
			files = p.music_language * MAX_LANGUAGE + music_file;	
			files <<= 8;
			floor_flag = 0;
			if(p.floor_enable)
				{
					UART_SendCommand(CMD_INSERT_MUL_FILE, FEED_BACK , files | floor_msg);//播放中文楼层	(位于文件夹ADVERT1)							
					play_timer = PLAYTIME_WAIT;
					PlayOverFlag = 0;
					play_status = 1;
				}
		}
	else if((updown_flag) && (updown_msg & (DOOR_UP | DOOR_DOWN)))
		{
			files = p.music_language * MAX_LANGUAGE + MESSAGE_FILE;
			files <<= 8;
			if(updown_msg & DOOR_UP)
				i = MAX_IO + MESSAGE_INDEX;
			else if(updown_msg & DOOR_DOWN)
				i = MAX_IO + MESSAGE_INDEX + 1;					
			if(updown_msg & (DOOR_UP | DOOR_DOWN))
				{
					updown_flag = 0;		
					if(p.arrow_enable)
						{
							UART_SendCommand(CMD_INSERT_MUL_FILE , FEED_BACK , files | i);	//播放上行
							play_timer = PLAYTIME_WAIT;
							PlayOverFlag = 0;
							play_status = 1;
						}
				}
		}
	else if((door_flag) && (door_msg))
		{
			j = door_msg - 1;			
			files = p.music_language * MAX_LANGUAGE + MESSAGE_FILE;
			files <<= 8;					
			i = MAX_IO + MESSAGE_INDEX + MAX_ARROW + j;
			door_msg = 0;
			door_flag = 0;
			if(p.door_enable)
				{
					UART_SendCommand(CMD_INSERT_MUL_FILE , FEED_BACK , files | i);					//播放状态信息
					play_timer = PLAYTIME_WAIT;
					PlayOverFlag = 0;
					play_status = 1;
				}
		}
	else if(warn_flag)		//播放告警信息
		{
			for(i = 0; i < sizeof(warn_msg); i++)
				{
					if(bit_select(warn_msg, i))
						{
							j = i;
							break;
						}
				}
			if(warn_msg)
				{				
					files = p.music_language * MAX_LANGUAGE + MESSAGE_FILE;
					files <<= 8;					
					i = MAX_IO + MESSAGE_INDEX + MAX_ARROW + j + 2;
					warn_flag = 0;	
					if(p.special_enable)
						{
							UART_SendCommand(CMD_INSERT_MUL_FILE , FEED_BACK , files | i);					//播放状态信息
							play_timer = PLAYTIME_WAIT;
							PlayOverFlag = 0;
							play_status = 1;
						}
				}
		}	
}

/**********************************************************************************
 - 功能描述： 输入DI检测并播放任务
 - 隶属模块： 外部
 - 参数说明： 无
 - 返回说明： 无
 - 注：	      
**********************************************************************************/
void Play_Input(void){
	uchar in_data, i;
	WORD files;
	static uchar in_data_old = 0;

	in_data = (~in & 0x07) >> 1;
 	if(in_data) 
 		{
 			if(in_data != in_data_old)
 				{
			 		DIO_Play_Flag = 1;
					in_data_old = in_data;
 				}
 		}
	else
		{
			DIO_Play_Flag = 0;
			in_data_old = 0;
		}
	if(in_data & DI1_IN)
		i = 0;
	else if(in_data & DI2_IN)
		i = 1;
	else if(in_data & DI3_IN)
		i = 2;
	else if(in_data & DI4_IN)
		i = 3;
	if(DIO_Play_Flag)
		{			
			files = p.music_language * MAX_LANGUAGE + MESSAGE_FILE;
			files <<= 8;					
			in_data = MESSAGE_INDEX + i;
			DIO_Play_Flag = 0;
			if(p.input_enable)
				{
					UART_SendCommand(CMD_INSERT_MUL_FILE , FEED_BACK , files | in_data);
					play_timer = PLAYTIME_WAIT;
					PlayOverFlag = 0;
					play_status = 1;
				}
		}
}

/**********************************************************************************
 - 函数说明：演示Demo程序
 - 隶属模块：内部
 - 参数说明：无
 - 返回说明：
**********************************************************************************/
void MP3_Demo(void){
	Play_Input();
	if(Time_xs_Flag)
  	{
  		Time_xs_Flag = 0; 
	  	if(paly_num == 1)		///1层到了
				{
					floor_msg = 1;		    
					floor_flag = 1;
					music_floor_file = 1;
				}		
			if(paly_num == 2)			//上行
				{
					updown_flag = 1;		
					updown_msg = 0x10;			
				}
			if(paly_num == 3)			///2层到了
				{
					floor_msg = 2;		    
					floor_flag = 1;
					music_floor_file = 1;
				}
			if(paly_num == 4)			///3层到了
				{
					floor_msg = 3;		   
					floor_flag = 1;
					music_floor_file = 1;
				}
			if(paly_num == 5)			///4层到了
				{
					floor_msg = 4;		    
					floor_flag = 1;
					music_floor_file = 1;
				}
			if(paly_num == 6)//下行
				{	
					updown_flag = 1;	
					updown_msg = 0x20;			
				}			 
			if(paly_num == 7)			///3层到了
				{
					floor_msg = 3;
					floor_flag = 1;
					music_floor_file = 1;
				}
			if(paly_num == 8)			///2层到了
				{
					floor_msg = 2;		    
					floor_flag = 1;
					music_floor_file = 1;
				}	
			if(paly_num == 9)			///1层到了
				{
					floor_msg =1;		    
					floor_flag = 1;
					music_floor_file = 1;
				}		  			
			if(DIO_Play_Flag == 0)
				Play_Task();
		}
}

void interrupt low_priority InterruptHandlerLow(void){
	BYTE i = 0, j;
	static BYTE toggle = 0;
	static BYTE t1_cnt = 0;
	static BYTE uart_rest_dly = 0;

	if(PIR1bits.RC1IF && PIE1bits.RC1IE)		
		{//串口中断		
			if(RCSTA1 & OERR)
				{//溢出
					i = RCREG1;
					i = RCREG1;
					mRx1Head = 0;
					RCSTA1bits.CREN = 0;
					RCSTA1bits.SPEN = 0;
					PIE1bits.RC1IE = 0;
					uart_rest_dly = 2;
				}
			else
				{					
					i = RCREG1;
					UartBuf.RxBuf[mRx1Head++] = i;
					Nop();Nop();Nop();Nop();Nop();Nop();
					if (PIR1bits.RC1IF)
						{
							i = RCREG1;
							UartBuf.RxBuf[mRx1Head++] = i;
						}
					if(UartBuf.RxBuf[0] != MP3_SEND_STARTBYTE_1)
						mRx1Head = 0;
					if((mRx1Head == 2) && (UartBuf.RxBuf[0] != MP3_SEND_STARTBYTE_1)
							&& (UartBuf.RxBuf[1] != MP3_SEND_STARTBYTE_2))
						mRx1Head = 0;
					if(mRx1Head >= USART_RX_BUFFER_SIZE)
						{//命令请求
							if(UartBuf.RxBuf[USART_RX_BUFFER_SIZE - 1] == MP3_SEND_STOPBYTE)
								{
									uart1_status_ok = 1;
									for(i = 0; i < USART_RX_BUFFER_SIZE; i++)
										sReceive_status[i] = UartBuf.RxBuf[i];
								}
							mRx1Head = 0;
						}
				}		
			PIR1bits.RC1IF = 0;			
		}
	
	if (PIR1bits.TX1IF && PIE1bits.TX1IE)
		{
			PIR1bits.TX1IF = 0;
			if (uart1_send_len)
				{
					TXREG1 = UartBuf.TxBuf[uart1_send_ro++];
					if (uart1_send_ro == USART_TX_BUFFER_SIZE)
						uart1_send_ro = 0;
					-- uart1_send_len;
				}
			else
				{
					uart1_send_busy = 0;
					PIE1bits.TX1IE	= 0;					
				}
		}
	
	if (INTCONbits.T0IF && INTCONbits.T0IE) 
		{//500ms 	定时器0中断处理函数
			INTCONbits.T0IF = 0;
			bTime_Fg.time_500ms = 1;
			TMR0L = T0L_500ms;
			TMR0H = T0H_500ms;			
			toggle = !toggle; 			

			if(music_led_no_good)
				LATDbits.LATD0 = !LATDbits.LATD0;
			
			if(play_timer)
				--play_timer;
			if(floor_timer)
				--floor_timer;

			if(toggle)
				{///1s
					bTime_Fg.time_1s = 1;
					com_can_work++; 				
					Timerxs_count++;
					if(Timerxs_count >= 10)
						{
							Time_xs_Flag = 1;
							Timerxs_count = 0;
							paly_num++;
							if(paly_num >= 9)
								paly_num = 1; 	
							bTime_Fg.time_10s = 1;
						}
					if(music_back_timer)
						music_back_timer--;
					
					if((in & 0x01) && music_init_finish)
						{//音频没有输出
							++music_output_timer;
							if(music_output_timer >= MUSIC_NO_OUTPUT_TIME)
								{
									++music_reset_number;
									if(music_reset_number <= MUSIC_RESET_MAX)
										reset_flag = 0;
									else		
										{//超过了复位次数，运行灯快速闪烁
											music_led_no_good = 1;
											music_reset_number = MUSIC_RESET_MAX;
										}
								}
						}
					else
						{
							music_output_timer = 0;
							music_reset_number = 0;
							music_led_no_good = 0;
						}
				}			
		} 	
	if (PIR1bits.TMR1IF && PIE1bits.TMR1IE)							/* check for Timer 1 overflow			*/
		{//5ms			
			++t1_cnt; 			
			PIR1bits.TMR1IF = 0;			
			TMR1H = T1H_5ms;
			TMR1L = T1L_5ms;
			bTime_Fg.time_5ms = 1;
			
			if((t1_cnt % 10) == 0)
				{//50ms
					bTime_Fg.time_50ms = 1;
				}
			if((t1_cnt % 20) == 0)
				{//100ms
					bTime_Fg.time_100ms = 1;
					if(can_inittime)
						--can_inittime; 
					if(UartRecvACK)
						{
							--UartRecvACK;
							if(!UartRecvACK)
								Resend_Flag = 1;
						}
					if (uart_rest_dly)
						{
							-- uart_rest_dly;
							if (!uart_rest_dly)
								{
									RCSTA1bits.CREN = 1;
									RCSTA1bits.SPEN = 1;
									i = RCREG1;
									i = RCREG1;
									PIE1bits.RC1IE = 1;
								}
						}
				}
			
			if(nmtwait)
				--nmtwait;
			
			//处理输入信号
			input [2] = input [1];							/* shift input variables				*/
			input [1] = input [0];
			input [0] = PORTC & 0x0F; 	//RC3 RC2 RC1 RC0
			for (i = 0; i < 4; i++)
				{
					j = 0x01 << i;
					if (((input [2] & j) == (input [1] & j)) &&
							((input [1] & j) == (input [0] & j)))
						{
							in &= ~j; 						/* set input valid						*/
							in |= (input [2] & j); 
						}
				}
		}
}


