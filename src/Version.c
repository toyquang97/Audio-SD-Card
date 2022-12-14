
//V1.2B  在丁工的基础上更改can.c程序，使用GTEC188系统的协议
//		 更改为整个寻址模式，并且文件夹的文件也经过整合

//	a.	SW2 拨码开关设定
//			bit2 bit1:  OFF OFF  中文播报
//								OFF ON	 英文播报
//								ON OFF	 中英文播报
//								ON ON	 中粤文播报
//			bit4 bit3:  OFF OFF  不播放背景音乐
//								OFF ON	 播报所有
//								ON OFF	 定时播报
//								ON ON	 运行 Demo 演示程序
//b. 外部I/O功能:
//		接收3个外部干节点信号: DI1- DI3，干节点功能可以自由定义
//c. 语音文件夹的设定:
//		c.1:  背景音乐文件夹:	01 文件夹: 中文背景音乐
//											02 文件夹: 英文背景音乐
//											03 文件夹: 中英文背景音乐
//											04 文件夹: 中粤文背景音乐
//											05 文件夹: 静音文件夹
//		c.2:  ADVERT1 - ADVERT3 文件夹: 中文楼层的语音信息
//			c.2.1: ADVERT1  标准的2位数字楼层信息
//			c.2.2: ADVERT2  标准的2位带字母楼层信息
//			c.2.3: ADVERT3  非标的2位3位楼层信息以及状态信息，001－200为楼层信息，201－240为状态信息
//		c.3:  ADVERT4 - ADVERT6 文件夹: 英文楼层的语音信息
//			c.3.1: ADVERT4  标准的2位数字楼层信息
//			c.3.2: ADVERT5  标准的2位带字母楼层信息
//			c.3.3: ADVERT6  非标的2位3位楼层信息以及状态信息，001－200为楼层信息，201－240为状态信息
//		c.4:  ADVERT7 - ADVERT9 文件夹: 中英文楼层的语音信息
//			c.4.1: ADVERT7  标准的2位数字楼层信息
//			c.4.2: ADVERT8  标准的2位带字母楼层信息
//			c.4.3: ADVERT9  非标的2位3位楼层信息以及状态信息，001－200为楼层信息，201－240为状态信息

//d. 增加语音芯片音频输出管脚的判定: PC0 = 1(无音频输出)     PC0 = 0(有音频输出)
//		当检测到 PC = 1(持续10S), 重新初始化语音芯片，重新检测PC0，持续10S高电平，继续初始化。最多初始化10次, 
//e.	改进开门关门报站信息的处理
//	f.	增加楼层到站播报的多报的问题: 在1楼，播1楼到了，2楼到了
//		解决方法: 楼层播报之间至少需要5s的延时

