#include "sys.h"
static idata u16 delay_tick = 0; //����ʵ�־�ȷ��ʱ��



//���ļĴ�����ʼ��
void sys_cpu_init()
{
	EA = 0;
	RS0 = 0;
	RS1 = 0;
	CKCON = 0x00;
	T2CON = 0x70;
	DPC = 0x00;
	PAGESEL = 0x01;
	D_PAGESEL = 0x02; //DATA RAM  0x8000-0xFFFF
	MUX_SEL	= 0xE0;		//��CAN�ӿ��Լ�UART2 3����
	PORTDRV = 1;		//����IO����������
	RAMMODE = 0;		//���DGUS�����ӿڼĴ���
	PORTDRV = 0x01;   //����ǿ��+/-8mA
	IEN0 = 0x00;      //�ر������ж�
	IEN1 = 0x00;
	IEN2 = 0x00;
	IP0 = 0x00;       //�ж����ȼ�Ĭ��
	IP1 = 0x00;
	P0MDOUT = 0x57;		
	P0 = 0xFC;		 	//����ߵ�ƽ
	WDT_OFF();      	//�رտ��Ź�
}


//��ʱ��2��ʼ��,��ʱ���Ϊ1ms
void sys_timer2_init()
{
	T2CON = 0x70;
	TH2 = 0x00;
	TL2 = 0x00;

	TRL2H = 0xBC;	//1ms�Ķ�ʱ��
	TRL2L = 0xCD;       

	IEN0 |= 0x20;	//������ʱ��2
	TR2 = 0x01;
	EA = 1;
}


//ϵͳ��ʼ��
void sys_init()
{
	sys_cpu_init();//���ļĴ�����ʼ��
	sys_timer2_init();//��ʱ��2��ʼ��
}


//���������ʱ,��λms
//����޸����Ż��ȼ�,��ô�˺����ڲ��Ĳ�����Ҫ���µ���
void sys_delay_about_ms(u16 ms)
{
	u16 i,j;
	for(i=0;i<ms;i++)
			for(j=0;j<3000;j++);    
}


//���������ʱ,��λus
//����޸����Ż��ȼ�,��ô�˺����ڲ��Ĳ�����Ҫ���µ���
void sys_delay_about_us(u8 us)
{
	u8 i,j;
	for(i=0;i<us;i++)
			for(j=0;j<5;j++);    
}


//���ö�ʱ��2���о�ȷ��ʱ,��λms
void sys_delay_ms(u16 ms)
{
	delay_tick = ms;
	while(delay_tick);
}


//��DGUS�е�VP��������
//addr:����ֱ�Ӵ���DGUS�еĵ�ַ
//buf:������
//len:��ȡ������,һ���ֵ���2���ֽ�
void sys_read_vp(u16 addr,u8* buf,u16 len)
{   
	u8 i; 
	
	i = (u8)(addr&0x01);
	addr >>= 1;
	ADR_H = 0x00;
	ADR_M = (u8)(addr>>8);
	ADR_L = (u8)addr;
	ADR_INC = 0x01;
	RAMMODE = 0xAF;
	while(APP_ACK==0);
	while(len>0)
	{   
		APP_EN=1;
		while(APP_EN==1);
		if((i==0)&&(len>0))   
		{   
			*buf++ = DATA3;
			*buf++ = DATA2;                      
			i = 1;
			len--;	
		}
		if((i==1)&&(len>0))   
		{   
			*buf++ = DATA1;
			*buf++ = DATA0;                      
			i = 0;
			len--;	
		}
	}
	RAMMODE = 0x00;
}


//дDGUS�е�VP��������
//addr:����ֱ�Ӵ���DGUS�еĵ�ַ
//buf:������
//len:���������ݵ�����,һ���ֵ���2���ֽ�
void sys_write_vp(u16 addr,u8* buf,u16 len)
{   
	u8 i;  
	
	i = (u8)(addr&0x01);
	addr >>= 1;
	ADR_H = 0x00;
	ADR_M = (u8)(addr>>8);
	ADR_L = (u8)addr;    
	ADR_INC = 0x01;
	RAMMODE = 0x8F;
	while(APP_ACK==0);
	if(i && len>0)
	{	
		RAMMODE = 0x83;	
		DATA1 = *buf++;		
		DATA0 = *buf++;	
		APP_EN = 1;		
		len--;
	}
	RAMMODE = 0x8F;
	while(len>=2)
	{	
		DATA3 = *buf++;		
		DATA2 = *buf++;
		DATA1 = *buf++;		
		DATA0 = *buf++;
		APP_EN = 1;		
		len -= 2;
	}
	if(len)
	{	
		RAMMODE = 0x8C;
		DATA3 = *buf++;		
		DATA2 = *buf++;
		APP_EN = 1;
	}
	RAMMODE = 0x00;
} 


//��ʱ��2�жϷ������
void sys_timer2_isr()	interrupt 5
{
	TF2=0;//�����ʱ��2���жϱ�־λ
	
	//��׼��ʱ����
	if(delay_tick)
		delay_tick--;
}

//����T5LӦ�ÿ���ָ��3.8��CAN�ӿڽ��г�ʼ��
void caninit()
{
	ADR_H = 0xFF;
	ADR_M = 0x00;
	ADR_L = 0x60;
	ADR_INC = 1;
	RAMMODE = 0x8F;		//д����
	while(!APP_ACK);
		DATA3 = 0x1A;
	DATA2 = 0x17;
	DATA1 = 0x0f;
	DATA0 = 0x00;//	������150K
//	DATA3 = 17;
//	DATA2 = 0x4c;
//	DATA1 = 0x1f;
//	DATA0 = 0x00;//	������125K
	APP_EN = 1;
	while(APP_EN);
	DATA3 = 0;
	DATA2 = 0;
	DATA1 = 0;
	DATA0 = 0;	 		//�������ռĴ���ACR
	APP_EN = 1;	  
	while(APP_EN);
	DATA3 = 0xFF;
	DATA2 = 0xFF;
	DATA1 = 0xFF;
	DATA0 = 0xFF;	 	//����AMR
	APP_EN = 1;	
	while(APP_EN);
	RAMMODE = 0;
	CAN_CR = 0xA0;
	while(CAN_CR&0x20);	//ִ������FF0060-FF0062����
	ECAN = 1;			//��CAN�ж�		
}



