#define	_CANBUS_C_
#include "AllHeader.h"

/**********************************************************************************
 - 功能描述： CAN初始化
 - 隶属模块： 外部
 - 参数说明： 无
 - 返回说明： 无
 - 注：	      波特率125kbit/s
**********************************************************************************/
void Can_init (BYTE baud_rate){
	BYTE i, j;
	INTCONbits.GIEH	= 0;							/* high priority interrupts disable		*/
	rc = 0;	   								   		/* clear all CAN variables				*/
	ri = 0;
	ro = 0;
	for (i = 0; i < RX_SIZE; i++)						/* clear rx buffer						*/
		{
			for (j = 0; j < 10; j++)
				rx[i][j] = 0;
		}
	for (j = 0; j < MAX_IO_TYPE; j++)
		virt_out[j] = 0;
	TRISBbits.TRISB2 	= 0;							/* Pin RB2 (CAN Tx) is output			*/
	TRISBbits.TRISB3	= 1;							/* Pin RB3 (CAN Rx) is input			*/
	CIOCONbits.ENDRHI	= 1;							/* Tx drives high when recessive state	*/
	CIOCONbits.CANCAP	= 0;							/* disable CAN capture					*/
	CANCON  = CAN_MODE_CONFIG;							/* set CAN configuration mode			*/
	while ((CANSTAT&CAN_MODE_BITS) != CAN_MODE_CONFIG);	/* wait till configuration mode is set	*/
	if (baud_rate == BPS_50K)
		BRGCON1 		= 0x89; 						/* SJW = 3TQ; BRP = 9 (1 Bit = 16 TQ);	*/
	else
		BRGCON1 		= 0x83; 						/* SJW = 3TQ; BRP = 3 (1 Bit = 16 TQ);	*/
	BRGCON2 			= 0xF4;	//0xFC;//0xBC;	/* SAM = 0; SEG1 = 3; PROP = 4			*/
	BRGCON3 			= 0x02;							/* SEG2 = 1 (2 TQ);						*/
	RXB0CON 			= 0x20;							/* only messages with standard ID		*/
	RXB1CON 			= 0x20;							/* only messages with standard ID		*/
	RXF0SIDL			= HSE_ID << 5;					/* Set acceptance filter 0 for buffer 0	*/
	RXF0SIDH			= HEARTBEAT;					/* buffer 0 only for Heartbeat HSE		*/
	RXF1SIDL			= HSE_ID << 5;					/* Set acceptance filter 1 for buffer 0	*/
	RXF1SIDH			= HEARTBEAT;					/* buffer 0 only for Heartbeat HSE		*/

	RXM0SIDL			= 0x00;
	RXM0SIDH			= 0xFE;							/* Set acceptance mask for buffer 0		*/

	RXF2SIDL			= 0x00;							/* Set acceptance filter 2 for buffer 1	*/
	RXF2SIDH			= PDO_OUT;						/* buffer 1 for virtual output messages	*/
	RXF3SIDL			= 0x00; 						/* Set acceptance filter 3 for buffer 1 */
	RXF3SIDH			= DOORS; 						/* buffer 1 for receive SDO's 		*/
	RXF4SIDL			= 0; 						/* Set acceptance filter 3 for buffer 1 */
	RXF4SIDH			= PDO_OUT;						/* buffer 1 for receive SDO's 		*/
	RXF5SIDL			= 0;					/* Set acceptance filter 1 for buffer 0	*/
	RXF5SIDH			= DOORS;					/* buffer 0 only for Heartbeat HSE		*/

	RXM1SIDL			= 0x00;							/* Set acceptance mask for buffer 1		*/
	RXM1SIDH			= 0xF0;							/* use only function code for filtering	*/
	CANCON  			= CAN_MODE_NORMAL;				/* set CAN normal mode					*/
	while ((CANSTAT & CAN_MODE_BITS) != CAN_MODE_NORMAL);	/* wait till normal mode is set			*/
 	TXB1SIDH 			= 0;	/* write ID bit 10 ... 3 for HEARTBEAT	*/
	TXB1SIDL 			= 0;					/* write ID bit  2 ... 0 for HEARTBEAT	*/
	TXB1DLC	 			= 0;							/* write data lenght code				*/
	TXB1D0   			= 0;						/* write data byte for HEARTBEAT		*/
	PIR5 				= 0;							/* clear all interrupt flags			*/
	PIE5 				= 0x23;							/* enable Error,RXB0 interrupt	*/
	IPR5 				= 0xFF;							/* all CAN interrupts high priority		*/
	INTCONbits.GIEH		= 1;							/* high priority interrupts enable		*/
}	

void interrupt high_priority InterruptHandlerHigh(void){
	BYTE i;
	BYTE lenght;
	BYTE * ptr;
	BYTE tempCANSTAT;
	
	com_can_work = 0;
	tempCANSTAT = CANSTAT & INTCODE;				/* read interrupt code					*/
	switch (tempCANSTAT)							/* read interrupt source				*/
		{
			case (RXB1INT)	: 							/* Message in message buffer 1			*/
				if (rc < RX_SIZE)							/* software buffer data not overrun 		*/
					{
						rx[ri][0] 	= RXB1SIDH & 0xF0;			/* read function code 				*/
						rx[ri][1] 	= (RXB1SIDL >> 5) + ((RXB1SIDH & 0x0F) << 3); /* node ID			*/
						if ((rx[ri][0] == PDO_OUT) || (rx[ri][0] == DOORS))
							{ 
								lenght = RXB1DLC	& 0x0F; 		/* read data lenght code				*/
								ptr = (BYTE*)&RXB1D0; 			/* set pointer to Data register of RXB1 */
								for (i = 0; i < lenght; i++)
									rx[ri][2 + i] = ptr[i]; 		/* read data bytes						*/
								if (ri >= (RX_SIZE - 1))				/* increment RX message write pointer */
									ri = 0;
								else
									ri++;
								rc++; 							/* increment message counter			*/
							}
					}
				RXB1CONbits.RXFUL 	= 0;					/* reset RX buffer full status bit		*/
				PIR5bits.RXB1IF 	= 0;					/* reset interrupt flag 				*/
				break;
				
			case (RXB0INT)	: 							/* Heartbeat Message from HSE 		*/
				hse_heartbeat = (RXB0SIDL >> 5) + (RXB0SIDH << 3) - 1; /* read node id (HSE number) 		*/
				RXB0CONbits.RXFUL = 0;						/* reset RX buffer full status bit		*/
				PIR5bits.RXB0IF = 0;						/* reset interrupt flag 				*/
				hse_heartbeat = 1;
				break;
			
			case (TXB0INT)	:								/* interrupt from TX buffer 0			*/
				PIR5bits.TXB0IF = 0;						/* reset interrupt flag					*/
				break;
				
			case (WAKEINT)	:
				PIR5bits.WAKIF = 0;							/* reset interrupt flag					*/
				break;
				
			case (TXB1INT)	:
				PIR5bits.TXB1IF = 0;						/* reset interrupt flag					*/
				break;
				
			case (TXB2INT)	:
				PIR5bits.TXB2IF = 0;						/* reset interrupt flag					*/
				break;
			case (ERRORINT)	:
				PIR5bits.ERRIF = 0;							/* reset interrupt flag					*/
				if (COMSTATbits.RXB1OVFL)				/* receive buffer 1 overflow			*/
					{
						COMSTATbits.RXB1OVFL = 0;				/* reset overflow flag					*/
					}
				if (COMSTATbits.RXB0OVFL)					/* receive buffer 0 overflow			*/
					{
						COMSTATbits.RXB0OVFL = 0;				/* reset overflow flag					*/
					}											/* no error code to set					*/
				if (COMSTATbits.TXBO)						/* bus off state						*/
					{
						if(!CanErr_Flag)
							{
								can_inittime = 20;
								CanErr_Flag = 1;	
							}
					}
				break;
		}
}

/************************************************************************************************/
/* 	read CAN message										                        			*/
/************************************************************************************************/
void read_rx (void){
	WORD	index;
	BYTE 	i, size;

	switch (rx [ro][0])
		{// message function code
			case (PDO_OUT) :						//receive PDO virtual output
			 	index = rx[ro][2];				//read function code
				if (index)								//function code > 0
					{
						if (nmtstate == OPERATIONAL)				// only in operational state
							{
								for (i = 0; i < MAX_IO_TYPE; i++)					// read output function
									virt_out [i] = rx[ro][i + 2];	// write to virtual output mapping
								set_output (virt_out);					// set physical outputs
							}
					}
				break;
				
			case (DOORS) :											// network management
				i = rx[ro][1];
				if(!i)
					{
						index = rx[ro][3];
						size = rx[ro][5];
						if(((index == CLOSE_DOOR) || (index == CLOSE_DOOR_L) ||
								(size == CLOSE_DOOR) || (size == CLOSE_DOOR_L)))//	 &&
							door_command |= LIFT_DOOR_CLOSE;
						else
							door_command &= ~LIFT_DOOR_CLOSE;
						if(((index == OPEN_DOOR) || (index == OPEN_DOOR_L) ||
								(size == OPEN_DOOR) || (size == OPEN_DOOR_L)))	// &&
							door_command |= LIFT_DOOR_OPEN;
						else
							door_command &= ~LIFT_DOOR_OPEN;
					}
				else if(i == DOOR1)
					door_state[0] = rx[ro][3];
				else if(i == DOOR2)
					door_state[1] = rx[ro][3];
				if(door_command)
					{
						if((door_command & LIFT_DOOR_CLOSE) &&
							((door_state[0] == (DOOR_CLOSING >> 8)) || (door_state[1] == (DOOR_CLOSING >> 8))))
							door_msg |= LIFT_DOOR_CLOSE;
						else if((door_command & LIFT_DOOR_OPEN) &&
							((door_state[0] == (DOOR_OPENING >> 8)) || (door_state[1] == (DOOR_OPENING >> 8))))
							door_msg |= LIFT_DOOR_OPEN;
						if(door_msg)
							{
								door_flag = 1;
								door_command = 0;
								door_state[0] = door_state[1] = 0;
							}
					}
				break;
			
			case (LSS) :										//LSS message for initialization
			case (NMT) :									/* network management 				*/
				break;
				
		}
	INTCONbits.GIEH		= 0;		// low priority interrupts disable
	if (ro >= (RX_SIZE - 1))		// increment RX message read pointer
		ro = 0;
	else
		ro++;
	rc--;											// decrement RX counter
	i = ri;
	size = rc;
	INTCONbits.GIEH		= 1;		// low priority interrupts enable
	if (i < ro)
		i += RX_SIZE;
	if ((i - ro) != (size % RX_SIZE))
		{
			Reset (); 						// force a reset
		}
}

BYTE fire_alarm = 0;
BYTE fire_evacuation = 0;
BYTE fire_state = 0;

void set_output (BYTE *virt){
	BYTE j, finish = 0;
	WORD i = 0;
	BYTE iotype;
	BYTE mes = 0;
	BYTE floor_ten = 0, floor_unit = 0;
	char buf[5];

	iotype = virt [IO_BASIC_FUNC];
	if (iotype == POSITION_INDICATOR)
		{//显示信息
			if (virt [IO_LIFT] == speech_lift)
	  		{// display message is for this lift
					floor_ten = virt [IO_DOOR];				// 1. digit; not CANopen compatible
					floor_unit = virt [IO_STATE];			// 2. digit; not CANopen compatible
					if(floor_ten == 0)
						floor_ten = 0x20;
					if(floor_unit == 0)
						floor_unit = 0x20;

					for(j = 0; j < DOWNLOAD_TOTAL_FILE; j++)
						{
							for(i = 0; i < MAX_FILE; i++)
								{
									sprintf (buf, "%s", (const char*)tFloorTab_standard[j].table[i]);
									if((floor_ten == buf[0]) && (floor_unit == buf[1]))
										{
											finish = 1;
											music_file = j + 1;
											floor = i + 1;
											break;
										}
									else if((buf[0] == '0') && (buf[1] == '0'))
										{
											break;
										}
								}
							if(finish)
								break;
						}
					if(!finish)
						{//未收索到标准楼层，从非标楼层表收索							
							for(j = 0; j < 26; j++)		
								{
									for(i = 0; i < 26; i++)
										{
											sprintf (buf, "%s", (const char*)tFloorTab_non_standard[j].table[i]);
											if((floor_ten == buf[0]) && (floor_unit == buf[1]))
												{
													finish = 1;
													music_file = 3;
													floor = j + FLOOR_NON_STANDARD;
													break;
												}
										}
									if(finish)
										break;
								}
						}
				}
		}
	else if (iotype == DIRECTION_IND)
		{//方向箭头
			if (virt [IO_LIFT] == speech_lift)				// display message is for this lift
			 	{
					if (!(virt [IO_STATE] & 0x01))
						mes = 0;
					else
						mes = (virt [IO_SUB_FUNC] & 0x33);
					if ((mes & 0x03) > 2)
						mes = 0;
					updown_msg = mes;
					play_arrow = 1; 				
				}
		}
	else if ((iotype == ARRIVAL_INDICATION) && (!floor_timer))
		{//到站灯/钟
			floor_msg = floor;						
			arrive_flag = 1;
			floor_timer = 10;
		}
	else if(iotype == SPECIAL_FUNC)
		{//特殊功能
			mes = virt[IO_SUB_FUNC];
			switch(mes)
				{
					case OUT_OF_ORDER:
						if(virt[IO_STATE])
							message_status |= LIFT_OUT_OF_ORDER;
						else
							message_status &= ~LIFT_OUT_OF_ORDER;
						break;
						
					case OVER_LOAD_STATE:	
						if(virt[IO_STATE])
							message_status |= LIFT_OVERLOAD;
						else
							message_status &= ~LIFT_OVERLOAD;
						break;
					
					case FIRE_ALARM:
					case FIRE_EVACUATION:
					case FIRE_STATE:
						switch(mes)
							{
								case FIRE_ALARM:
									fire_alarm = virt[IO_STATE];
									break;
								case FIRE_EVACUATION:
									fire_evacuation = virt[IO_STATE];
									break;
								case FIRE_STATE:
									fire_state = virt[IO_STATE];
									break;
							}
						if(fire_alarm | fire_evacuation | fire_state)
							message_status |= LIFT_FIRESERVICE;
						else
							message_status &= ~LIFT_FIRESERVICE;
						break;
						
					case FAULT_SIGNAL: 
						if(virt[IO_STATE])
							message_status |= LIFT_FAULT;
						else
							message_status &= ~LIFT_FAULT;
						break;
						
					case UPS_EVACUATION:
						if(virt[IO_STATE])
							message_status |= LIFT_EMERGENCY_POWER;
						else
							message_status &= ~LIFT_EMERGENCY_POWER;
						break;

					case KEY_ADVANCED:
						if(virt[IO_STATE])
							message_status |= LIFT_VIP_RUN;
						else
							message_status &= ~LIFT_VIP_RUN;
						break;
						
					case FULL_LOAD_STATE: 
						if(virt[IO_STATE])
							message_status |= LIFT_OCCUPIED;
						else
							message_status &= ~LIFT_OCCUPIED;
						break;
						
					case KEY_SPECIAL:
						if(virt[IO_STATE])
							message_status |= LIFT_CAR_PRIORITY;
						else
							message_status &= ~LIFT_CAR_PRIORITY;
						break;
					
				}		
			warn_msg = message_status;
			if(warn_msg)
				{
					if(warn_msg != warn_msg_old)
						{
							warn_msg_old = warn_msg; 
							warn_flag = 1; 
						} 
				}
			else
				{
					warn_msg = 0;
					warn_msg_old = warn_msg; 
					warn_flag = 0; 
				}
		}
	if(arrive_flag)			
		{	
			arrive_flag = 0;
			floor_flag = 1;						        
		}
	
	if((updown_msg != updown_msg_old) && (updown_msg & 0x33))
		{
			if(play_arrow)
				{
					updown_msg_old = updown_msg;
					play_arrow = 0;
					updown_flag = 1;
				}								        
		}
}

