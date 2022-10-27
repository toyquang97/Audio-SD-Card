#ifndef	_SUBPROG_H_
#define	_SUBPROG_H_
//定义常量

#define DOOR_UP  		0x10
#define DOOR_DOWN  	0x20

#define DOOR_OLO    0x87
#define DOOR_OOO  	0x10
#define DOOR_VRU  	0x06

#define DI1_IN  	0x01
#define DI2_IN  	0x02
#define DI3_IN  	0x04
#define DI4_IN  			0x08

//**************************************************************************************************
//	Key definition
//**************************************************************************************************
#define KEY_ESC		0x001B			// simulated by pressing '#' more than 1 sec
#define KEY_UP		0x0430			// simulated by pressing '*' more than 1 sec

#define SM_KEY_ENT			0x0001
#define SM_KEY_MENU			0x0002
#define SM_KEY_UP			0x0004
#define	SM_KEY_ENT_MENU		0x0008
#define	SM_KEY_ENT_UP		0x0010
#define	SM_KEY_MENU_UP		0x0020
#define	SM_KEY_ALL			0x0040
#define	LM_KEY_FLAG			0x8000
#define LM_KEY_ENT			0x8001
#define LM_KEY_MENU			0x8002
#define LM_KEY_UP			0x8004
#define	LM_KEY_ENT_MENU		0x8008
#define	LM_KEY_ENT_UP		0x8010
#define	LM_KEY_MENU_UP		0x8020
#define	LM_KEY_ALL			0x8040

#define OPEN_RUN_LED() 		LATDbits.LATD0 = 0
#define CLOSE_RUN_LED() 	LATDbits.LATD0 = 1

#ifdef	_SUBPROG_C_
void delay_ms(unsigned int tick);
BYTE check_night_func(void);

//定义变量，声明函数

#else
//共享变量函数
extern  void delay_ms(unsigned int tick);
extern  void LedWork(void);
extern  void Function_Config(void);
extern  BYTE read_eeprom (WORD address);
extern  BYTE write_eeprom (WORD address, BYTE value);
extern  BYTE check_timer_for_night(void);
extern  BYTE check_night_func(void);

#endif
#endif

