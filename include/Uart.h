#ifndef __UART_H
#define __UART_H

//#define	OSC_FREQ		16000000
//#define BAUD_9600	         (OSC_FREQ / (9600 * 64) - 1)
#define BAUD_9600        			25
#define MSG_TF	          		2
#define RESEND_ENABLE_FLAG  	1		//重发使能标志
#define RESEND_MAX_CNT  			3		//最大重发次数
#define WAIT_ACK_500MS   			10		//等待应答时间300ms	
#define RESEND_FLAG    			1
#define RESEND_DATALEN   		8

#define RESET_CMD    0x0c
#define STOP_CMD     0x15

#define PALY_BUSY    0x01
#define NOT_BUSY     0x00

#define	OERR		1
#define	MP3_SEND_STARTBYTE_1		0x7E
#define	MP3_SEND_STARTBYTE_2		0xFF
#define	MP3_SEND_STOPBYTE				0xEF

/* USART Buffer Defines */
#define	STATUS_BUF_SIZE		40
#define USART_RX_BUFFER_SIZE 10 /* 1,2,4,8,16,32,64,128 or 256 bytes */
#define USART_RX_BUFFER_MASK ( USART_RX_BUFFER_SIZE - 1 )
#define USART_TX_BUFFER_SIZE 200 /* 1,2,4,8,16,32,64,128 or 256 bytes */
#define USART_TX_BUFFER_MASK ( USART_TX_BUFFER_SIZE - 1 )

//命令字定义
#define CMD_PLAY_NEXT_UP				0x01		//播放上一首
	//参数: 无
#define CMD_PLAY_NEXT_DN				0x02		//播放下一首
	//参数: 无
#define CMD_PLAY_DESIRE				0x03		//指定曲目播放
	//参数: 1--2999
#define CMD_VOLUMN_ADD				0x04		//增大音量
	//参数: 无
#define CMD_VOLUMN_SUBB				0x05		//减小音量
	//参数: 无
#define CMD_VOLUMN_SETTING			0x06		//设置音量
	//参数: 0--30等级
#define CMD_PLAY_EQO						0x07		//设定播放的音效
	//参数: Normal/Pop/Rock/Jazz/Classic/Bass
#define CMD_PLAY_CIRCLE_ONE			0x08		//单曲循环播放
	//参数: 0--2999
#define CMD_PLAY_DEVICE				0x09		//设定播放的设备
	//参数: U/SD/AUX/SLEEP/FLASH
#define CMD_SLEEPING						0x0A		//进入休眠模式
	//参数: 无
#define CMD_RESET							0x0C		//复位芯片
	//参数: 无
#define CMD_PLAY_GO						0x0D		//开始播放
	//参数: 无
#define CMD_PLAY_PAUSE					0x0E		//暂停播放
	//参数: 无
#define CMD_PLAY_FILE						0x0F		//指定文件夹播放  
	//参数: 1-10(需要自己设定)
#define CMD_PLAY_EXTEND				0x10		//扩音设置 
	//参数: DH: 1开扩音   DL: 0--31  设置增益大小
#define CMD_PLAY_ALL_LOOP			0x11		//全部循环播放
	//参数: 1: 循环播放    0: 停止循环播放
#define CMD_PLAY_MUSIC_FILE			0x12		//指定文件夹曲目
	//参数: 0--9999
#define CMD_INSERT_ONE_FILE			0x13		//插播广告
	//参数: 0--9999
#define CMD_INSERT_STOP				0x15		//停止插播，播放背景音乐
	//参数: 无
#define CMD_PLAY_STOP					0x16		//停止播放
	//参数: 无
#define CMD_PLAY_FILE_LOOP			0x17		//指定文件夹循环播放  
	//参数: 01-99  文件夹
#define CMD_PLAY_RANDOM				0x18		//随机播放  
	//参数: 01-99  文件夹
#define CMD_PLAY_CIRCLE				0x19		//循环播放  
	//参数: 0: 启动循环播放    1: 关闭循环播放
#define CMD_PLAY_DAC						0x1A		//开启或关闭DAC  
	//参数: 0: 启动DAC    1: 关闭DAC
#define CMD_INSERT_MUL_FILE			0x25			//多文件夹插播

//应答设置
#define NO_FEED_BACK 	0x00
#define FEED_BACK    	0x01

//背景音乐文件夹定义
#define MUSIC_BACK_FILE_EN	1
#define MUSIC_BACK_FILE_CN	2
#define MUSIC_BACK_FILE_ECN	3
#define MUSIC_BACK_FILE_YY	4
#define MUSIC_BACK_FILE_NO	5

enum PLAYDEVICE_TYPE{
  PLAYDEVICE_NULL = 0 ,
	PLAYDEVICE_UD,
  PLAYDEVICE_TF	,
	PLAYDEVICE_PC,
	PLAYDEVICE_FLASH,
  PLAYDEVICE_SLEEP,  
  PLAYDEVICE_MAX ,
} ;

/*****************************************************************************************************
 - 这里用枚举的方式，将所有需要用到的指令列出来
 - 有疑惑可以参照具体的模块手册。
 - 以下的命名均可以通过翻译明白命令的具体含义
*****************************************************************************************************/
enum UARTCMD_TYPE{ 
//主机发送的命令
  UARTCMD_NEXT_FILE = 0x01,				//01	下一曲
		//参数: 无
  UARTCMD_PREV_FILE , 						//02	上一曲   
		//参数: 无
  UARTCMD_NUM_POINT , 						//03	指定曲目
		//参数: 1--2999
  UARTCMD_VOL_UP,     						//04	音量+
		//参数: 无
  UARTCMD_VOL_DOWN,   						//05	音量-
		//参数: 无
  UARTCMD_VOL_POINT,  						//06	指定音量
		//参数: 0--30等级
  UARTCMD_EQ_POINT,   						//07	指定EQ
		//参数: Normal/Pop/Rock/Jazz/Classic/Bass
  UARTCMD_LOOP_PLAY,							//08	指定循环播放曲目
		//参数: 0--2999
  UARTCMD_PLAYDEVICE,							//09	指定播放设备
		//参数: U/SD/AUX/SLEEP/FLASH
  UARTCMD_SLEEP_MODE,							//0A	进入睡眠低功耗
		//参数: 无
  NO_UARTCMD,										//0B	保留命令
  UARTCMD_RESET,									//0C	复位命令
		//参数: 无
  UARTCMD_MUSIC_PLAY,							//0D	播放命令
		//参数: 无
  UARTCMD_MUSIC_PAUSE,						//OE	暂停播放
		//参数: 无
  UARTCMD_MUSIC_FOLDER,						//0F	指定文件夹播放
		//参数: 1-10(需要自己设定)
  UARTCMD_KALA ,									//10	扩音设置
		//参数: DH: 1开扩音 	DL: 0--31  设置增益大小
  UARTCMD_PLAYALL,								//11	全部循环播放
		//参数: 1: 循环播放 	 0: 停止循环播放
  UARTCMD_MUSIC_MP3,							//12 指定文件夹曲目播放
		//参数: 0--9999
	UARTCMD_INSERT_ONE_FILE,					//13	插播广告		
		//参数: 0--9999
	UARTCMD_INSERT_STOP = 0x15,			//15 停止插播广告,转而播放背景音乐	
		//参数: 无
	UARTCMD_PLAY_STOP,							//16	停止播放
		//参数: 无
	UARTCMD_PLAY_FILE_LOOP,					//17	指定文件夹循环播放  
		//参数: 01-99  文件夹
	UARTCMD_PLAY_RANDOM,						//18	随机播放  
		//参数: 01-99  文件夹
	UARTCMD_PLAY_CIRCLE,						//19	循环播放  
		//参数: 0: 启动循环播放 	 1: 关闭循环播放
	UARTCMD_PLAY_DAC,								///1A	开启或关闭DAC 
		//参数: 0: 启动DAC		1: 关闭DAC
	UARTCMD_INSERT_MUL_FILE	= 0x25,	//25	多文件夹插播
		//参数: 0--9999

//语音芯片返回来的信息以及主机的查询命令
	UARTQUR_CARD_INSERT = 0x3A,			///3A	设备插入消息
	UARTQUR_CARD_OUTPUT,						///3B	设备拔出消息
	UARTQUR_STAY_UD,								///3C		U盘播放完毕返回的数据
	UARTQUR_STAY_TF ,								///3D		TF卡播放完毕返回的数据
	UARTQUR_STAY_FLASH ,						///3E		FLASH播放完毕返回的数据
	UARTQUR_START_UP ,							///3F		上电返回的数据
	UARTQUR_ERR ,									//40		发送错误信息
	UARTQUR_ACK,										//41		发送的应答信息
  UARTQUR_STATUS ,								//43		主机查询当前状态
  UARTQUR_VOL ,									//43		主机查询音量
  UARTQUR_EQ ,										//44		主机查询当前EQ
  UARTQUR_PLAYMODE ,							//45		主机查询当前播放模式
  UARTQUR_VER , 									//46		主机查询当前软件版本
  UARTQUR_UDISK_TOTAL_FILES,				//47		主机查询U盘总文件数
  UARTQUR_TF_TOTAL_FILES,					//48		主机查询TF卡总文件数
  UARTQUR_FLASH_TOTAL_FILES,				//49		主机查询FLASH总文件数
  UARTQUR_STAY,										///4A		保留
  UARTQUR_UDISK_CONTENTS,					///4B		主机查询U盘目录			
  UARTQUR_TF_CONTENTS,						///4C		主机查询TF卡目录			
  UARTQUR_FLASH_CONTENTS,    			///4D		主机查询FLASH目录			
  UARTQUR_TOTAL_FILES,    				///4E		主机查询文件夹的总文件数
  UARTQUR_FILES,    							///4F		主机查询总文件数
} ;

//接收的数据定义
enum UART_RECV_MSG{
  UART_RECV_IDLE = 0 ,//串口接收空闲模式
  UART_RECV_VER ,  
  UART_RECV_LENTH,
  UART_RECV_CMD,
  UART_RECV_FEEDBACK ,
  UART_RECV_DATAH,
  UART_RECV_DATAL,
  UART_RECV_CHECKSUMH,
  UART_RECV_CHECKSUML,    
  UART_RECV_OVER,
} ;
//UART_RECV_MSG uart1_receive_buf;

#ifdef	_UART_C_

//数据接收变量定义
struct StructBuf{
	BYTE RxBuf[USART_RX_BUFFER_SIZE];		//接收缓冲区
	BYTE TxBuf[USART_TX_BUFFER_SIZE];		//接收缓冲区
};
struct StructBuf UartBuf;	//UART0-UART3
BYTE sReceive_status[USART_RX_BUFFER_SIZE];
volatile uchar paly_num = 0;
uchar DIO_Play_Flag = 0;

BYTE mRx1Head = 0; 			//接收缓冲区的头指针
BYTE uart1_send_busy = 0;
BYTE uart1_send_to = 0;
BYTE uart1_send_ro = 0;
BYTE uart1_send_len = 0;
BYTE uart1_status_ok = 0;
uchar Send_buf[10] = {0};

uchar Resend_Flag = 0 ;//重发标志
uchar ResendCount = 0 ;//重发次数

uchar PlayDevice = 0;//当前播放的设备
uchar OnlineDevice = 0 ;//在线的设备

BYTE uart1_err_status = 0;
uchar UartRecvACK = 0;
uchar reset_flag = 0 ;//系统状态

BYTE PlayOverFlag = 0; //播放完毕标志
uchar Time_xs_Flag = 0;
uchar Timerxs_count = 0;
uchar play_status = 0;

void UART_Init(uint baud);
void UART_SendOneByte(uchar byte);
void UART_SendBuf(uchar* array, uchar len);
void UART_Handle(uchar *pr);
void UART_CheckSum( uchar *Str, uchar len);
void UART_SendCommand(uchar CMD ,uchar feedback , uint dat);

void MP3_init(void);
void Play_Input(void);
void Play_Task(void);

#else
extern BYTE uart1_status_ok;

extern uchar reset_flag;
extern uchar UartRecvACK;
extern BYTE PlayOverFlag; //播放完毕标志
extern BYTE play_status;

extern uchar DIO_Play_Flag;

extern BYTE sReceive_status[USART_RX_BUFFER_SIZE];

extern void UART_Init(uint baud);
extern void UART_Handle(uchar *pBuf);
extern void UART_ReSend(void);
extern void UART_ResetCheck(void);
extern void UART_SendCommand(uchar CMD ,uchar feedback , uint dat);

extern void MP3_init(void);
extern void Play_Task(void);
extern void MP3_Demo(void);
extern void Play_Input(void);

#endif
#endif

