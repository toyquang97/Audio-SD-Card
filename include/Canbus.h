
#ifndef _CANBUS_H_
#define	_CANBUS_H_
/************************************************************************************************/
/* Definitions for configuration register CANCON												*/
/************************************************************************************************/
#define CAN_MODE_BITS		0xE0		   				/* Mode bits in configuration register	*/
#define CAN_MODE_CONFIG		0x80		   				/* Configuration mode					*/
#define CAN_MODE_NORMAL		0x00		   				/* Normal mode							*/
#define CAN_MODE_LISTEN		0x60

#define ID_SEND_CAN      0x01
#define ID_RECE_CAN1     0x0400
#define ID_RECE_CAN2     0x0200
#define CAN_TX_MAX  8
#define CAN_RX_MAX  8

/************************************************************************************************/
/* Definitions for interrupt code in CANSTAT register											*/
/************************************************************************************************/
#define INTCODE				0x0E		   				/* Mask for interrupt code bits			*/
#define WAKEINT				0x0E		   				/* Wake up interrupt					*/
#define RXB0INT				0x0C		   				/* RX buffer 0 interrupt				*/
#define RXB1INT				0x0A		   				/* RX buffer 1 interrupt				*/
#define TXB0INT				0x08		   				/* TX buffer 0 interrupt				*/
#define TXB1INT				0x06		   				/* TX buffer 1 interrupt				*/
#define TXB2INT				0x04		   				/* TX buffer 2 interrupt				*/
#define ERRORINT			0x02		   				/* error interrupt						*/
#define NOINT				0x00		   				/* kein interrupt						*/

#define RX_SIZE						50

/************************************************************************************************/
/* Definitions for bit timing register					 										*/
/************************************************************************************************/
#define	BAUD_250k			0x01						/* BRP for 250 kBaud					*/
#define	BAUD_125k			0x03						/* BRP for 125 kBaud					*/

/************************************************************************************************/
/* Definitions for baud rate according to CANopen		 										*/
/************************************************************************************************/
#define	BPS_125K			0
#define	BPS_50K				1
#define	BPS_25K				2
#define	BPS_250K			3

#define	CTRL_A				0x0001
#define	CTRL_D				0x0004

#define	SUB_INDEX_HSE		1			//Virtual termial-->Device
#define	SUB_INDEX_G790	2			//Device-->Virtual termial

#define	C_BAUDRATE_LISTEN_18S	2800//18
#define	C_MINIMAM_TIMES			4

#define	MAX_MESSAGE_NUM	32
	#define	LIFT_DOOR_OPEN					BIT_0		//6		电梯开门
	#define	LIFT_DOOR_CLOSE					BIT_1		//7		电梯关门
	#define	LIFT_FIRESERVICE 				BIT_2		//8		消防服务
	#define	LIFT_OVERLOAD					BIT_3		//9		电梯超载
	#define LIFT_OUT_OF_ORDER				BIT_4		//10		电梯暂停使用
	#define LIFT_EMERGENCY_POWER 	BIT_5		//11		电梯应急救援
	#define LIFT_FAULT							BIT_6		//12 		电梯故障
	#define LIFT_VIP_RUN						BIT_7		//13		VIP 运行
	#define LIFT_OCCUPIED						BIT_8		//14 		电梯独立服务
	#define LIFT_CAR_PRIORITY				BIT_10 	//15		轿厢优先运行
	#define LIFT_MAINTAIN 					BIT_11 	//15		电梯维护	
	#define LIFT_LEAVE_CAR 					BIT_12 	//15		请离开轿厢
	#define LIFT_DESTINA 						BIT_13 	//15		请选择目标楼层
	#define LIFT_WAIT_FOR_RESCUE 		BIT_14 	//15		请等待救援
	#define LIFT_USE_PHOTON 				BIT_15 	//15		请使用救援电话

#ifdef	_CANBUS_C_

BYTE volatile rc;			// RX message counter
BYTE volatile ro;			// pointer for FIFO read
BYTE volatile ri;			// pointer for FIFO write

BYTE volatile rx[RX_SIZE][10];	// RX message buffer

void set_output (BYTE *virt);

#else
extern	BYTE volatile rx[RX_SIZE][10];	// RX message buffer
extern	BYTE volatile rc;			// RX message counter

extern	void Can_init (BYTE baud_rate);
extern	void read_rx (void);

#endif
#endif


