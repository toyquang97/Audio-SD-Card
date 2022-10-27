#ifndef __UART_H
#define __UART_H

//#define	OSC_FREQ		16000000
//#define BAUD_9600	         (OSC_FREQ / (9600 * 64) - 1)
#define BAUD_9600        			25
#define MSG_TF	          		2
#define RESEND_ENABLE_FLAG  	1		//�ط�ʹ�ܱ�־
#define RESEND_MAX_CNT  			3		//����ط�����
#define WAIT_ACK_500MS   			10		//�ȴ�Ӧ��ʱ��300ms	
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

//�����ֶ���
#define CMD_PLAY_NEXT_UP				0x01		//������һ��
	//����: ��
#define CMD_PLAY_NEXT_DN				0x02		//������һ��
	//����: ��
#define CMD_PLAY_DESIRE				0x03		//ָ����Ŀ����
	//����: 1--2999
#define CMD_VOLUMN_ADD				0x04		//��������
	//����: ��
#define CMD_VOLUMN_SUBB				0x05		//��С����
	//����: ��
#define CMD_VOLUMN_SETTING			0x06		//��������
	//����: 0--30�ȼ�
#define CMD_PLAY_EQO						0x07		//�趨���ŵ���Ч
	//����: Normal/Pop/Rock/Jazz/Classic/Bass
#define CMD_PLAY_CIRCLE_ONE			0x08		//����ѭ������
	//����: 0--2999
#define CMD_PLAY_DEVICE				0x09		//�趨���ŵ��豸
	//����: U/SD/AUX/SLEEP/FLASH
#define CMD_SLEEPING						0x0A		//��������ģʽ
	//����: ��
#define CMD_RESET							0x0C		//��λоƬ
	//����: ��
#define CMD_PLAY_GO						0x0D		//��ʼ����
	//����: ��
#define CMD_PLAY_PAUSE					0x0E		//��ͣ����
	//����: ��
#define CMD_PLAY_FILE						0x0F		//ָ���ļ��в���  
	//����: 1-10(��Ҫ�Լ��趨)
#define CMD_PLAY_EXTEND				0x10		//�������� 
	//����: DH: 1������   DL: 0--31  ���������С
#define CMD_PLAY_ALL_LOOP			0x11		//ȫ��ѭ������
	//����: 1: ѭ������    0: ֹͣѭ������
#define CMD_PLAY_MUSIC_FILE			0x12		//ָ���ļ�����Ŀ
	//����: 0--9999
#define CMD_INSERT_ONE_FILE			0x13		//�岥���
	//����: 0--9999
#define CMD_INSERT_STOP				0x15		//ֹͣ�岥�����ű�������
	//����: ��
#define CMD_PLAY_STOP					0x16		//ֹͣ����
	//����: ��
#define CMD_PLAY_FILE_LOOP			0x17		//ָ���ļ���ѭ������  
	//����: 01-99  �ļ���
#define CMD_PLAY_RANDOM				0x18		//�������  
	//����: 01-99  �ļ���
#define CMD_PLAY_CIRCLE				0x19		//ѭ������  
	//����: 0: ����ѭ������    1: �ر�ѭ������
#define CMD_PLAY_DAC						0x1A		//������ر�DAC  
	//����: 0: ����DAC    1: �ر�DAC
#define CMD_INSERT_MUL_FILE			0x25			//���ļ��в岥

//Ӧ������
#define NO_FEED_BACK 	0x00
#define FEED_BACK    	0x01

//���������ļ��ж���
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
 - ������ö�ٵķ�ʽ����������Ҫ�õ���ָ���г���
 - ���ɻ���Բ��վ����ģ���ֲᡣ
 - ���µ�����������ͨ��������������ľ��庬��
*****************************************************************************************************/
enum UARTCMD_TYPE{ 
//�������͵�����
  UARTCMD_NEXT_FILE = 0x01,				//01	��һ��
		//����: ��
  UARTCMD_PREV_FILE , 						//02	��һ��   
		//����: ��
  UARTCMD_NUM_POINT , 						//03	ָ����Ŀ
		//����: 1--2999
  UARTCMD_VOL_UP,     						//04	����+
		//����: ��
  UARTCMD_VOL_DOWN,   						//05	����-
		//����: ��
  UARTCMD_VOL_POINT,  						//06	ָ������
		//����: 0--30�ȼ�
  UARTCMD_EQ_POINT,   						//07	ָ��EQ
		//����: Normal/Pop/Rock/Jazz/Classic/Bass
  UARTCMD_LOOP_PLAY,							//08	ָ��ѭ��������Ŀ
		//����: 0--2999
  UARTCMD_PLAYDEVICE,							//09	ָ�������豸
		//����: U/SD/AUX/SLEEP/FLASH
  UARTCMD_SLEEP_MODE,							//0A	����˯�ߵ͹���
		//����: ��
  NO_UARTCMD,										//0B	��������
  UARTCMD_RESET,									//0C	��λ����
		//����: ��
  UARTCMD_MUSIC_PLAY,							//0D	��������
		//����: ��
  UARTCMD_MUSIC_PAUSE,						//OE	��ͣ����
		//����: ��
  UARTCMD_MUSIC_FOLDER,						//0F	ָ���ļ��в���
		//����: 1-10(��Ҫ�Լ��趨)
  UARTCMD_KALA ,									//10	��������
		//����: DH: 1������ 	DL: 0--31  ���������С
  UARTCMD_PLAYALL,								//11	ȫ��ѭ������
		//����: 1: ѭ������ 	 0: ֹͣѭ������
  UARTCMD_MUSIC_MP3,							//12 ָ���ļ�����Ŀ����
		//����: 0--9999
	UARTCMD_INSERT_ONE_FILE,					//13	�岥���		
		//����: 0--9999
	UARTCMD_INSERT_STOP = 0x15,			//15 ֹͣ�岥���,ת�����ű�������	
		//����: ��
	UARTCMD_PLAY_STOP,							//16	ֹͣ����
		//����: ��
	UARTCMD_PLAY_FILE_LOOP,					//17	ָ���ļ���ѭ������  
		//����: 01-99  �ļ���
	UARTCMD_PLAY_RANDOM,						//18	�������  
		//����: 01-99  �ļ���
	UARTCMD_PLAY_CIRCLE,						//19	ѭ������  
		//����: 0: ����ѭ������ 	 1: �ر�ѭ������
	UARTCMD_PLAY_DAC,								///1A	������ر�DAC 
		//����: 0: ����DAC		1: �ر�DAC
	UARTCMD_INSERT_MUL_FILE	= 0x25,	//25	���ļ��в岥
		//����: 0--9999

//����оƬ����������Ϣ�Լ������Ĳ�ѯ����
	UARTQUR_CARD_INSERT = 0x3A,			///3A	�豸������Ϣ
	UARTQUR_CARD_OUTPUT,						///3B	�豸�γ���Ϣ
	UARTQUR_STAY_UD,								///3C		U�̲�����Ϸ��ص�����
	UARTQUR_STAY_TF ,								///3D		TF��������Ϸ��ص�����
	UARTQUR_STAY_FLASH ,						///3E		FLASH������Ϸ��ص�����
	UARTQUR_START_UP ,							///3F		�ϵ緵�ص�����
	UARTQUR_ERR ,									//40		���ʹ�����Ϣ
	UARTQUR_ACK,										//41		���͵�Ӧ����Ϣ
  UARTQUR_STATUS ,								//43		������ѯ��ǰ״̬
  UARTQUR_VOL ,									//43		������ѯ����
  UARTQUR_EQ ,										//44		������ѯ��ǰEQ
  UARTQUR_PLAYMODE ,							//45		������ѯ��ǰ����ģʽ
  UARTQUR_VER , 									//46		������ѯ��ǰ����汾
  UARTQUR_UDISK_TOTAL_FILES,				//47		������ѯU�����ļ���
  UARTQUR_TF_TOTAL_FILES,					//48		������ѯTF�����ļ���
  UARTQUR_FLASH_TOTAL_FILES,				//49		������ѯFLASH���ļ���
  UARTQUR_STAY,										///4A		����
  UARTQUR_UDISK_CONTENTS,					///4B		������ѯU��Ŀ¼			
  UARTQUR_TF_CONTENTS,						///4C		������ѯTF��Ŀ¼			
  UARTQUR_FLASH_CONTENTS,    			///4D		������ѯFLASHĿ¼			
  UARTQUR_TOTAL_FILES,    				///4E		������ѯ�ļ��е����ļ���
  UARTQUR_FILES,    							///4F		������ѯ���ļ���
} ;

//���յ����ݶ���
enum UART_RECV_MSG{
  UART_RECV_IDLE = 0 ,//���ڽ��տ���ģʽ
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

//���ݽ��ձ�������
struct StructBuf{
	BYTE RxBuf[USART_RX_BUFFER_SIZE];		//���ջ�����
	BYTE TxBuf[USART_TX_BUFFER_SIZE];		//���ջ�����
};
struct StructBuf UartBuf;	//UART0-UART3
BYTE sReceive_status[USART_RX_BUFFER_SIZE];
volatile uchar paly_num = 0;
uchar DIO_Play_Flag = 0;

BYTE mRx1Head = 0; 			//���ջ�������ͷָ��
BYTE uart1_send_busy = 0;
BYTE uart1_send_to = 0;
BYTE uart1_send_ro = 0;
BYTE uart1_send_len = 0;
BYTE uart1_status_ok = 0;
uchar Send_buf[10] = {0};

uchar Resend_Flag = 0 ;//�ط���־
uchar ResendCount = 0 ;//�ط�����

uchar PlayDevice = 0;//��ǰ���ŵ��豸
uchar OnlineDevice = 0 ;//���ߵ��豸

BYTE uart1_err_status = 0;
uchar UartRecvACK = 0;
uchar reset_flag = 0 ;//ϵͳ״̬

BYTE PlayOverFlag = 0; //������ϱ�־
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
extern BYTE PlayOverFlag; //������ϱ�־
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

