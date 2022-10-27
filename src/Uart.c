
#define	_UART_C_
#include "AllHeader.h"	

/**********************************************************************************
 - ���������� ����1��ʼ��
 - ����ģ�飺 �ⲿ
 - ����˵���� ��
 - ����˵���� ��
 - ע��	      ����9600������
**********************************************************************************/
void UART_Init(uint baud){	
	TRISCbits.TRISC6 = 0; 	//�첽��ʽ
	TRISCbits.TRISC7 = 1;
	
	BAUDCON1bits.BRG16 = 0;		//8λģʽ
	SPBRG1 = baud;
	SPBRGH1 = baud >> 8;	
	SPBRGH1 = 0;
	SPBRGH1 = 0;

	TXSTA1 = 0x20;
	RCSTA1 = 0x90;		//ʹ�ܽ�����
	RCSTA1 = 0x90;
	BAUDCON1bits.TXCKP = 0;
	
	PIR1bits.TX1IF = 0;
	IPR1bits.TX1IP = 0; 	//���ڷ���Ϊ�����ȼ�
	PIE1bits.TX1IE = 0;
	IPR1bits.RC1IP = 0; 	//���ڽ���Ϊ�����ȼ�
	PIE1bits.RC1IE = 1;
}

/**********************************************************************************
 - ���������� ���ڷ���һ���ֽ�
 - ����ģ�飺 �ⲿ
 - ����˵����
 - ����˵����
 - ע��	      
***********************************************************************************/
void UART_SendOneByte(uchar byte){
	TXREG1 = byte;
	while(! TXSTA1bits.TRMT);	 
}

/**********************************************************************************
 - ���������� ���ڷ���һ���ֽ�
 - ����ģ�飺 �ⲿ
 - ����˵����
 - ����˵����
 - ע��	      
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
 - ���������� ���������ط�������
 - ����ģ�飺 �ⲿ
 - ����˵����
 - ����˵����
 - ע��
**********************************************************************************/
void UART_ReSend(void){
	if(Resend_Flag)
		{//��� Resend_Flag = 1 ʱ�������û���յ�ACK ���ط�
			ResendCount ++;
			if(ResendCount > RESEND_MAX_CNT)
				{
					ResendCount  = 0;
					Resend_Flag  = 0;
					return;
				}
			UART_SendBuf(Send_buf, sizeof(Send_buf));
			UartRecvACK = WAIT_ACK_500MS ;//�趨�ȴ�Ӧ���ʱ��[300ms] 
		}
}

/**********************************************************************************
 - ���������� ���play�ܽ��Ƿ�æ
 - ����ģ�飺 �ⲿ
 - ����˵����
 - ����˵����
 - ע��
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
 - �������������У��
 - ����ģ�飺
 - ����˵����
 - ����˵����
 - ע��      ��У���˼·����
             ���͵�ָ�ȥ����ʼ�ͽ��������м��6���ֽڽ����ۼӣ����ȡ����
             ���ն˾ͽ����յ���һ֡���ݣ�ȥ����ʼ�ͽ��������м�������ۼӣ��ټ��Ͻ��յ���У��
             �ֽڡ��պ�Ϊ0.�����ʹ�����յ���������ȫ��ȷ��
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
 - ���������� �������ⷢ������[�������ƺͲ�ѯ]
 - ����ģ�飺 �ⲿ
 - ����˵���� CMD:��ʾ����ָ������ָ�����������ѯ�����ָ��
              feedback:�Ƿ���ҪӦ��[0:����ҪӦ��1:��ҪӦ��]
              data:���͵Ĳ���
 - ����˵����
 - ע��       
********************************************************************************************/
void UART_SendCommand(uchar CMD, uchar feedback, uint dat){
	Send_buf[UART_RECV_IDLE] = MP3_SEND_STARTBYTE_1;    	//�����ֽ� 
	Send_buf[UART_RECV_VER] = MP3_SEND_STARTBYTE_2;    	//�����ֽ� 
  Send_buf[UART_RECV_LENTH] = 0x06;    								//����
  Send_buf[UART_RECV_CMD] = CMD;     									//����ָ��
  Send_buf[UART_RECV_FEEDBACK] = feedback;							//�Ƿ���Ҫ����
  Send_buf[UART_RECV_DATAH] = (uchar)(dat >> 8);				//datah
  Send_buf[UART_RECV_DATAL] = (uchar)dat;     				//datal
  Send_buf[UART_RECV_OVER] = MP3_SEND_STOPBYTE;
  UART_CheckSum(&Send_buf[UART_RECV_VER], 6);   		   	//У��
  UART_SendBuf(Send_buf, sizeof(Send_buf));
  UartRecvACK = WAIT_ACK_500MS ;										//�趨�ȴ�Ӧ���ʱ��[300ms] 
}

/***************************************************************************
 - ���������� ������������
 - ����ģ�飺 �ڲ�
 - ����˵���� *pr ������0��Ҫ���յ����ݵĵ�ַָ��
 - ����˵���� ��
 - ע��	
***************************************************************************/
void UART_Handle(uchar *pBuf){
  WORD checksum = 0;
	BYTE command, i;
	BYTE data_h, data_l;

	for(i = 1; i < 7; i++)
		checksum += pBuf[i];
	checksum += (((WORD)pBuf[7] << 8) | pBuf[8]);
	if(!checksum)
		{//У����OK
			UartRecvACK = 0 ;
			Resend_Flag  = 0;
			ResendCount  = 0; 						
			command = pBuf[UART_RECV_CMD];
			data_h = pBuf[UART_RECV_DATAH];
			data_l = pBuf[UART_RECV_DATAL];
		  switch(command)
			  {			  	
		      case (UARTQUR_START_UP):				//ģ���ϵ緵�ص�����
		        OnlineDevice = data_l ;
						PlayDevice = data_l ;
		        break ;

		      case (UARTQUR_CARD_INSERT):					//�豸����
		        if(PlayDevice == data_l)
							OnlineDevice = PlayDevice ;		//�����豸�Ѿ�ȥ��
		        break;
						
		      case (UARTQUR_CARD_OUTPUT):					//�豸�γ�
		        if(PlayDevice == data_l)
							OnlineDevice = PLAYDEVICE_NULL ;						
		        break;
		                  
		      case (UARTQUR_STAY_TF):					//TF��ǰ��Ŀ�������
						play_status = 0;
		        break ;
						
		      case (UARTQUR_ERR):				//ģ�鷵�صĴ���
			      uart1_err_status = data_l;
						if(data_l == 0x01)
							{//æ��־
								//do nothing
							}
						else if((data_l == 0x02)	||	//����˯��ģʽ
								(data_l == 0x05)	||			//�ļ�������Χ
								(data_l == 0x06))					//�Ҳ���ָ���ļ�
							{
								reset_flag = 1;		//���¶Դ��ڽ��г�ʼ��
							}
						else if((data_l == 0x03)	||	//һ֡����û�н������
								(data_l == 0x04))					//У��ͳ���,���·���
							{
								Resend_Flag = 1;		//�����ط���־
							}
						else
							{//do nothing
							}
		        break ;  
						
					case (UARTQUR_ACK):					//�յ�Ӧ��ACK
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
			delay_ms(500);				//��λ�ȴ���ʱ�䲻����̫�̣�������Ҫ300ms����
//�������ദ��	
			UART_Init(BAUD_9600);		//���³�ʼ��UART�Լ�MP3
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
//��ʼ��ʱ����ȱ�ٲ��ŵ�ָ��
	if(p.music_back_type != PLAY_NO_BACKMUSIC) 
		UART_SendCommand(CMD_PLAY_FILE_LOOP , FEED_BACK , ((p.music_language % (MAX_LANGUAGE + 1)) + 1));//ָ���ļ���01--04ѭ������
	else 
		UART_SendCommand(CMD_PLAY_FILE_LOOP , FEED_BACK , MUSIC_BACK_FILE_NO);//ָ�������ļ���05ѭ������
	delay_ms(1000);	
	LATDbits.LATD1 = 0;
	LATDbits.LATD2 = 0;
}

/**********************************************************************************
 - ���������� ��������������
 - ����ģ�飺 �ⲿ
 - ����˵���� ��
 - ����˵���� ��
 - ע��	      
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
					UART_SendCommand(CMD_INSERT_MUL_FILE, FEED_BACK , files | floor_msg);//��������¥��	(λ���ļ���ADVERT1)							
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
							UART_SendCommand(CMD_INSERT_MUL_FILE , FEED_BACK , files | i);	//��������
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
					UART_SendCommand(CMD_INSERT_MUL_FILE , FEED_BACK , files | i);					//����״̬��Ϣ
					play_timer = PLAYTIME_WAIT;
					PlayOverFlag = 0;
					play_status = 1;
				}
		}
	else if(warn_flag)		//���Ÿ澯��Ϣ
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
							UART_SendCommand(CMD_INSERT_MUL_FILE , FEED_BACK , files | i);					//����״̬��Ϣ
							play_timer = PLAYTIME_WAIT;
							PlayOverFlag = 0;
							play_status = 1;
						}
				}
		}	
}

/**********************************************************************************
 - ���������� ����DI��Ⲣ��������
 - ����ģ�飺 �ⲿ
 - ����˵���� ��
 - ����˵���� ��
 - ע��	      
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
 - ����˵������ʾDemo����
 - ����ģ�飺�ڲ�
 - ����˵������
 - ����˵����
**********************************************************************************/
void MP3_Demo(void){
	Play_Input();
	if(Time_xs_Flag)
  	{
  		Time_xs_Flag = 0; 
	  	if(paly_num == 1)		///1�㵽��
				{
					floor_msg = 1;		    
					floor_flag = 1;
					music_floor_file = 1;
				}		
			if(paly_num == 2)			//����
				{
					updown_flag = 1;		
					updown_msg = 0x10;			
				}
			if(paly_num == 3)			///2�㵽��
				{
					floor_msg = 2;		    
					floor_flag = 1;
					music_floor_file = 1;
				}
			if(paly_num == 4)			///3�㵽��
				{
					floor_msg = 3;		   
					floor_flag = 1;
					music_floor_file = 1;
				}
			if(paly_num == 5)			///4�㵽��
				{
					floor_msg = 4;		    
					floor_flag = 1;
					music_floor_file = 1;
				}
			if(paly_num == 6)//����
				{	
					updown_flag = 1;	
					updown_msg = 0x20;			
				}			 
			if(paly_num == 7)			///3�㵽��
				{
					floor_msg = 3;
					floor_flag = 1;
					music_floor_file = 1;
				}
			if(paly_num == 8)			///2�㵽��
				{
					floor_msg = 2;		    
					floor_flag = 1;
					music_floor_file = 1;
				}	
			if(paly_num == 9)			///1�㵽��
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
		{//�����ж�		
			if(RCSTA1 & OERR)
				{//���
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
						{//��������
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
		{//500ms 	��ʱ��0�жϴ�����
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
						{//��Ƶû�����
							++music_output_timer;
							if(music_output_timer >= MUSIC_NO_OUTPUT_TIME)
								{
									++music_reset_number;
									if(music_reset_number <= MUSIC_RESET_MAX)
										reset_flag = 0;
									else		
										{//�����˸�λ���������еƿ�����˸
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
			
			//���������ź�
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


