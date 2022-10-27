
#ifndef	_TYPEDEF_H_
#define	_TYPEDEF_H_

typedef unsigned char 	uchar;
typedef unsigned int 	uint;
typedef unsigned short 	ushort;
typedef unsigned long	ulong;

typedef unsigned long		DWORD;
typedef unsigned int		WORD;
typedef unsigned char		BYTE;
typedef signed long			SDWORD;
typedef signed int			SWORD;
typedef signed char			SBYTE;


#define	DOWNLOAD_TOTAL_FILE		3			//一共3个文件夹，File1
#define	MAX_FILE		260								//每个文件夹最多240个文件
#define	MAX_IO			4									//一共4个干节点输入
#define	MAX_ARROW			2							
#define	MESSAGE_INDEX	200						//状态信息文件夹每种语言占用80个文件
#define	MESSAGE_FILE		3						//状态信息文件夹每种语言占用80个文件
#define	FLOOR_NON_STANDARD		221		//从 221 文件开始为非标楼层
#define	MESSAGE_NON_STANDARD		247		//从 247 文件开始为非标状态信息


//拨码开关的配置值
#define	MAX_LANGUAGE						3
	#define CHINESE             		0x00		//中文
	#define ENGLISH             		0x01		//英文
	#define CHINESE_AND_ENGLISH   	0x02		//中英文
	#define CHINESE_AND_YUEYU    		0x03		//中英粤
#define	MAX_PLAY_MODE					4
	#define PLAY_NO_BACKMUSIC 		0x00		//不播放背景音乐
	#define PLAY_ALL            		0x01		//播放所有
//	#define PLAY_NO_ARROW_MOVING		0x01		//不播放运行信号
	#define PLAY_BACKMUSIC_TIMER		0x02		//一段时间后取消
	#define PLAY_DEMO             0x03		//运行DEMO

#define	MAX_FLOOR		64
#define	MAX_SPECIAL	40
	

#endif

