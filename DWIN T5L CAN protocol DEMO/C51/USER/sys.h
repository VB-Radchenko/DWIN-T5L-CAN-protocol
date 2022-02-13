#ifndef __SYS_H__
#define __SYS_H__
#include "t5los8051.h"

//类型重定义
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned long   u32;
typedef signed char     s8;
typedef signed short    s16;
typedef signed long     s32;


//看门狗宏定义
#define	WDT_ON()				MUX_SEL|=0x02		//开启看门狗
#define	WDT_OFF()				MUX_SEL&=0xFD		//关闭看门狗
#define	WDT_RST()				MUX_SEL|=0x01		//喂狗

//系统主频和1ms定时数值定义
#define FOSC     				206438400UL
#define T1MS    				(65536-FOSC/12/1000)

 


//函数申明
void sys_init(void);
void sys_delay_about_ms(u16 ms);
void sys_delay_about_us(u8 us);
void sys_delay_ms(u16 ms);
void sys_read_vp(u16 addr,u8* buf,u16 len);
void sys_write_vp(u16 addr,u8* buf,u16 len);
void caninit();
void CAN_tx();
void CAN_rx();


#endif


