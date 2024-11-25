#include <REGX51.H>
//#include "Delay.h"

unsigned char count=0;
unsigned char seg[8] = {16,16,16,16,16,16,16,16}; //初始化全黑
unsigned char second=0,tensecond=0,minute=0,tenminute=0;
bit read_flag;  //while1 的读取标志位
sfr AUXR=0x8e;
unsigned char urdat=0;



void Delay(unsigned int xms)
{
	unsigned char i, j;
	while(xms--)
	{
		i = 2;
		j = 239;
		do
		{
			while (--j);
		} while (--i);
	}
}



unsigned char code SMG_duan[] = {                        
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E,
// black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0xFF, 0xBF, 0x89, 0xE1, 0x8F, 0xC7, 0xC8, 0xA3, 0x8C, 0xC1, 0x87, 0xC2, 0x98, 0xAF, 0xC8, 0x91,
    0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0xB9
};    // 0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code SMG_wei[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码

void sys_init()
{
   P2 = (P2&0x1f)|0x80;
	 P0=0xff;
	 P2 = (P2&0x1f)|0xa0;
	 P0=0x00;
	 P2 = (P2&0x1f);
}

void UartInit()
{
	TMOD = 0X20;
	TH1 = 0XFD;      //9600BFS
	TL1 = 0XFD;
	TR1 = 1;   //开定时器
	
	SCON = 0X50;
	AUXR = 0X00;
	
	ES = 1; 
	EA = 1;
}

void InitTimer0() //定时器初始化
{
	TMOD = 0X01;
	TH0 = (65535 - 4000) / 256;
	TL0 = (65535 - 4000) % 256;
	
	ET0 = 1;
	EA = 1;
	TR0 = 1;
}



void Seg_Show(){
	static unsigned char num;
	
	 P0=0xff;          P2=(P2 & 0x1f) | 0xe0;       P2=(P2 & 0x1f);
	P0=SMG_wei[num];   P2=(P2 & 0x1f) | 0xc0;       P2=(P2 & 0x1f);
	P0=SMG_duan[seg[num]];P2=(P2 & 0x1f) | 0xe0;   P2=(P2 & 0x1f);    //是因为共阳
	
	num++;
	num%=8; //循环
}

void Seg_Set(unsigned char t1,t2,t3,t4,t5,t6,t7,t8){
	seg[0]=t1;
	seg[1]=t2;
	seg[2]=t3;
	seg[3]=t4;
	seg[4]=t5;
	seg[5]=t6;
	seg[6]=t7;
	seg[7]=t8;
	
}


void main()
{
   sys_init();
	 InitTimer0();
	 UartInit();
	while(1)
	{	 
		  if(read_flag)
			{ 
/////////////////							
			SBUF = tenminute + '0'; 
      while(!TI); 
      TI = 0; 
      SBUF = minute + '0'; 
      while(!TI); 
      TI = 0; 
      SBUF = ':'; 
      while(!TI); 
      TI = 0; 
      SBUF = tensecond + '0'; 
      while(!TI); 
      TI = 0; 
      SBUF = second + '0'; 
      while(!TI); 
      TI = 0; 
///////////////////////////////
				 read_flag=0;
			   second++;
			   if(second>9)
				 {
				   tensecond++;
					 if(tensecond>5)  //因为tensecond++在上，会出现7
					 {
					    minute++;
						 if(minute>9)
						 {
						    tenminute++;
							   if(tenminute>5)
								 {
								    tenminute=0;
								 }
								 minute=0;
						 }
						 tensecond=0;
					 }
					  second=0;
				 }
			}
			Seg_Set(16,16,16,16,tenminute,minute,tensecond,second);
			Seg_Show();
			Delay(10);
	 }


}

void outerserver() interrupt 4
{
   if(TI==1)
	{
	  TI=0;
	}
	if(RI == 1)
	{
		RI = 0;
    urdat=SBUF;                   //在此写入代码
		switch (urdat)
		{
			case '1':TR0=~TR0 ;break;
			case '2':TR0=~TR0 ;break;
			case '3':tenminute=minute=tensecond=second=0 ;break;
		  default: SBUF=0x0e; P0=0xff;
		}
	}

}


void ServiceTime0() interrupt 1   //中断服务程序
{
	
	TH0 = (65535 - 4000) / 256;   //4ms
	TL0 = (65535 - 4000) % 256;
	count++;

	if(count%250==0)
	{
		read_flag=1;
		count=0;
	}
}	
