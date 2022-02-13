#include "sys.h"
#define VP 0X3004

unsigned char xdata can_buf[12];
unsigned char xdata ID = 0;
unsigned char xdata rx_done = 0;
u8 i;

void CAN_tx();
void CAN_rx();
void main(void)
{   
	sys_init();//ϵͳ��ʼ��
	caninit();//CAN��ʼ��
	while(1)
	{ 
		EA=1;
		sys_read_vp(0x3000,can_buf,5);
		if(can_buf[9]==5)
		{	
			CAN_tx();
			can_buf[8] = 0;
			can_buf[9] = 0;
			can_buf[10]	= 0;
			can_buf[11]	= 0;
			sys_write_vp(0x3004,(u8*)(&can_buf[8]),1);
			sys_delay_about_ms(20);			
		}
		CAN_rx();	
	}

}

//CAN����
void CAN_tx()
{

	EA = 0;
	ADR_H = 0xFF;
	ADR_M = 0x00;
	ADR_L = 0x64;
	ADR_INC = 1;
	RAMMODE = 0x8F;		//д����
	while(!APP_ACK);
	DATA3 = 8;			//֡�೤�����Լ�����
	DATA2 = 0;
	DATA1 = 0;
	DATA0 = 0;	 		
	APP_EN = 1;
	while(APP_EN);		//д��RTR,IDE,DLC������
	DATA3 = ID;
	DATA2 = 0;
	DATA1 = 0;
	DATA0 = 0;	 		
	APP_EN = 1;
	while(APP_EN);		//д��ID����
	DATA3 = can_buf[0];
	DATA2 = can_buf[1];
	DATA1 = can_buf[2];
	DATA0 = can_buf[3];	 		
	APP_EN = 1;
	while(APP_EN);		//д�뷢������ǰ4�ֽ�
	DATA3 = can_buf[4];
	DATA2 = can_buf[5];
	DATA1 = can_buf[6];
	DATA0 = can_buf[7];	 		
	APP_EN = 1;
	while(APP_EN);		//д�뷢�����ݺ�4�ֽ�
	EA = 1;
	RAMMODE = 0;
	CAN_CR |= 0x04;		//��������			
}


//CAN_rx����
void CAN_rx()
{
		if(rx_done == 1)
		{
			rx_done = 0;
			sys_write_vp(0x3000,can_buf,4);		
		}	
}



void can_Isr() interrupt 9
{
	EA = 0;
	if((CAN_IR&0xC0) == 0xC0)
	{
		CAN_IR &= 0x3F;	//���Զ��֡���λ
	}
	else if((CAN_IR&0xC0) == 0x40)
	{
		CAN_IR &= 0xBF;	//�������֡���λ
		ADR_H = 0xFF;
		ADR_M = 0x00;
		ADR_L = 0x6A;
		ADR_INC = 1;
		RAMMODE = 0xAF;		//������
		while(!APP_ACK);
		APP_EN = 1;
		while(APP_EN);
		can_buf[0] = DATA3;
		can_buf[1] = DATA2;
		can_buf[2] = DATA1;
		can_buf[3] = DATA0;
		APP_EN = 1;
		while(APP_EN);
		can_buf[4] = DATA3;
		can_buf[5] = DATA2;
		can_buf[6] = DATA1;
		can_buf[7] = DATA0;
		APP_EN = 1;
		while(APP_EN);
		RAMMODE = 0;
		rx_done = 1;	
	}
	if((CAN_IR&0x20) == 0x20)
	{
		CAN_IR &= 0xDF;	//��շ���֡���λ
	}
	if((CAN_IR&0x10) == 0x10)
	{
		CAN_IR &= 0xEF;	//��ս���������λ
	}
	if((CAN_IR&0x08) == 0x08)
	{
		CAN_IR &= 0xF7;	//��մ�����λ
		CAN_ET &= 0xE0;	//��մ�����	
	}
	if((CAN_IR&0x04) == 0x04)
	{
		CAN_IR &= 0xFB;	//����ٲ�ʧ�ܱ��λ
		CAN_CR |= 0x04;	//������������	
	}
	EA = 1;  
}












