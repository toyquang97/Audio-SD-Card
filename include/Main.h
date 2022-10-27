
#ifndef _MAIN_H_
#define _MAIN_H_

#define	BIT_0		0x01
#define	BIT_1		0x02
#define	BIT_2		0x04
#define	BIT_3		0x08
#define	BIT_4		0x10
#define	BIT_5		0x20
#define	BIT_6		0x40
#define	BIT_7		0x80
#define	BIT_8		0x0100
#define	BIT_9		0x0200
#define	BIT_10	0x0400
#define	BIT_11	0x0800
#define	BIT_12	0x1000
#define	BIT_13	0x2000
#define	BIT_14	0x4000
#define	BIT_15	0x8000
#define	BIT_16	0x00010000
#define	BIT_17	0x00020000
#define	BIT_18	0x00040000
#define	BIT_19	0x00080000
#define	BIT_20	0x00100000
#define	BIT_21	0x00200000
#define	BIT_22	0x00400000
#define	BIT_23	0x00800000
#define	BIT_24	0x01000000
#define	BIT_25	0x02000000
#define	BIT_26	0x04000000
#define	BIT_27	0x08000000
#define	BIT_28	0x10000000
#define	BIT_29	0x20000000
#define	BIT_30	0x40000000
#define	BIT_31	0x80000000

#define	_TRUE		0x55
#define	_FALSE		0xAA

#define	WD_MERKER			0xAA						// restart by watchdog timer
#define	BS_MERKER			0x55						// restart by CAN bus off
#define	RC_MERKER			0x5A						// restart by error in rc pointer
#define	ID_MERKER			0xA5

#define	MAX_IO_TYPE	7
#define	IO_BASIC_FUNC		0							/* basic function of input / output		*/
#define	IO_SUB_FUNC			1							/* sub function of input / output		*/
#define	IO_LIFT				2							/* lift number of input / output		*/
#define	IO_FLOOR			3							/* floor number of input / output		*/
#define	IO_DOOR				4							/* door number of input / output		*/
#define	IO_STATE			5							/* state of input / output				*/
#define	IO_ENABLE		6
#define	IO_ACK				6//7							/* acknowledgement marker for calls		*/
#define	ENABLE				1
#define	DISABLE			0

#define	T0START_1TO32		0x84						// Start Timer 0, prescaler 1 : 32/
#define	T0H_500ms			0x0C						// Timer 0 preset value 500 ms/
#define	T0L_500ms			0x00

#define	T1START_1TO1	0x31					// Start Timer 1, 8 prescaler
#define	T1H_5ms				0xF6//0xEC						// Timer 1 preset value 100 ms
#define	T1L_5ms				0x3C//0x78

#define	T2START_1TO48		0x16//0x0E//0x16						// Start Timer 2, prescaler 1 : 16, postscale 1 : 3
#define	T2_3ms				0xFF

#define	PUSHTIME					12
#define	HEARTBEATTIME		4
#define	MAX_LIFT					8

#define bit_select(B, i)	((B >> i) & 1)
#define bit_set(B, i)		(B |= (1 << i))
#define bit_reset(B, i)		(B &= ~(1 << i))

#define	MUSIC_POWER_OUTPUT()	LATDbits.LATD3 = 1

#define	MUSIC_NO_OUTPUT_TIME	20		//音频没有输出的时间检测(s)
#define	MUSIC_RESET_MAX			10		//最大的复位次数

#define	PLAYTIME_WAIT	8

#ifdef  MAIN_C

BYTE nmtstate;					// network management state
BYTE com_can_work = 0;
BYTE can_inittime = 0;
uchar CanErr_Flag = 0;
WORD volatile nmtwait = 0;			// waiting time for 1. heartbeat
BYTE volatile input[3]; 		// input variables for debouncing
BYTE volatile in; 			// variable for input after debouncing
BYTE virt_out[MAX_IO_TYPE];				// virtual output (last changed)
BYTE hse_heartbeat = 0;

BYTE mValumn_Switch = 0;				// set valumn
	//	1: Large Valumn
	//	2: Small Valumn

uchar arrive_flag = 0;
uchar updown_msg = 0;
uchar updown_msg_old = 0;
uchar floor_msg = 0xFF;
uchar floor = 0xFF;
uchar music_floor_file = 0;
WORD warn_msg = 0;
WORD warn_msg_old = 0;
uchar warn_flag = 0;
uchar door_msg = 0;
uchar door_flag = 0;
uchar floor_flag = 0;
uchar updown_flag = 0;
uchar play_arrow = 0;
BYTE music_file = 0;
BYTE	door_state[2] = {0xFF, 0xFF};
uchar door_command = 0;
BYTE	play_timer = 0;

//电梯的状态信息
WORD message_status = 0;

WORD music_back_timer = 0;
BYTE music_volumn = 0;
BYTE music_back_status = 0;
BYTE speech_lift = 0;

//音频输出检测标志
BYTE music_output_timer = 0;
BYTE music_reset_number = 0;
BYTE music_init_finish = 0;
BYTE music_led_no_good = 0;

BYTE floor_timer = 0;

struct Parameter{
	BYTE music_volumn;			//音量设置
	BYTE music_back_type;
	BYTE music_language;
	BYTE music_timer;	
	BYTE floor_enable;			//楼层到站播报使能位    	1-->使能播报      	0-->禁止播报
	BYTE arrow_enable;			//运行方向播报使能位    	1-->使能播报       0-->禁止使能
	BYTE door_enable;			//开关门播报使能位    	1-->使能播报       0-->禁止使能
	BYTE input_enable;			//干触点输入播报使能位  	1-->使能播报       0-->禁止使能
	BYTE special_enable;		//状态信息播报使能位    	1-->使能播报       0-->禁止使能
}
struct Parameter p;

//定义位标志
struct{
	BYTE time_5ms	:1;
	BYTE time_10ms	:1;		
	BYTE time_50ms	:1;
	BYTE time_100ms	:1;
	BYTE time_500ms	:1;
	BYTE time_1s	:1;			///1s 标志位
	BYTE time_10s:1;
	BYTE time_20s:1;
}bTime_Fg;


#else
extern  struct{
	BYTE time_5ms	:1;
	BYTE time_10ms	:1;		
	BYTE time_50ms	:1;
	BYTE time_100ms	:1;
	BYTE time_500ms	:1;
	BYTE time_1s	:1;			///1s 标志位
	BYTE time_10s	:1;
	BYTE time_20s:1;
}bTime_Fg;

extern  struct Parameter{
	BYTE music_volumn;			//音量设置
	BYTE music_back_type;
	BYTE music_language;
	BYTE music_timer;	
	BYTE floor_enable;			//楼层到站播报使能位    	1-->使能播报      	0-->禁止播报
	BYTE arrow_enable;			//运行方向播报使能位    	1-->使能播报       0-->禁止使能
	BYTE door_enable;			//开关门播报使能位    	1-->使能播报       0-->禁止使能
	BYTE input_enable;			//干触点输入播报使能位  	1-->使能播报       0-->禁止使能
	BYTE special_enable;		//状态信息播报使能位    	1-->使能播报       0-->禁止使能
}p;

extern  BYTE speech_lift;
extern  BYTE virt_out[MAX_IO_TYPE];				// virtual output (last changed)

extern  BYTE com_can_work;
extern  BYTE can_inittime;
extern  BYTE volatile in;
extern  WORD volatile nmtwait;			// waiting time for 1. heartbeat
extern  BYTE volatile input[3]; 		// input variables for debouncing

extern  BYTE mValumn_Switch;				// set liop can ID mode
extern	uchar floor_flag;
extern	uchar floor_msg;
extern	uchar floor;
extern	uchar arrive_flag;
extern	uchar updown_msg;
extern	uchar updown_msg_old;
extern	uchar updown_flag;
extern	WORD warn_msg;
extern	WORD warn_msg_old;
extern	uchar warn_flag;
extern	uchar door_flag;
extern	uchar door_msg;
extern	WORD message_status;
extern	BYTE	door_state[2];
extern	uchar door_command;
extern	uchar play_arrow;
extern	BYTE	play_timer;

extern  BYTE nmtstate;					// network management state
extern	uchar hse_heartbeat;

extern  uchar CanErr_Flag;
extern  BYTE floor_timer;

extern  BYTE music_file;
extern	uchar music_floor_file;
extern  WORD music_back_timer;
extern	BYTE music_volumn;
extern  BYTE music_back_status;
extern  BYTE music_output_timer;
extern  BYTE music_reset_number;
extern  BYTE music_init_finish;
extern  BYTE music_led_no_good;

#endif
#endif

