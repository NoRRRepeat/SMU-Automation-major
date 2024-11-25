#include <REGX51.H>
#include "Delay.h"

unsigned char count=0;
unsigned char seg[8] = {16,16,16,16,16,16,16,16}; //初始化全黑
unsigned char second=0,tensecond=0,minute=0,tenminute=0;
bit read_flag;  //while1 的读取标志位
bit pause_flag = 0;
sbit S4 = P3 ^ 3;
sbit S5 = P3 ^ 2;

unsigned char code SMG_duan[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code SMG_wei[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码

void sys_init()
{
   P2 = (P2&0x1f)|0x80;
	 P0=0xff;
	 P2 = (P2&0x1f)|0xa0;
	 P0=0x00;
	 P2 = (P2&0x1f);
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
	P0=~SMG_duan[seg[num]];P2=(P2 & 0x1f) | 0xe0;   P2=(P2 & 0x1f); //~是因为共阳
	
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
	while(1)
	{
		  
				if(S4==0)
				{
					Delay(20);
					if(S4==0)
					{
				  tenminute=minute=tensecond=second=0;
			   	}
		  	}
				
			 if (S5 == 0) {
         Delay(20);
         if (S5 == 0) {
            pause_flag = ~pause_flag; // S5????
            TR0 = !pause_flag; 
         }
      }
					 
		  if(read_flag)
			{ 
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
			Delay(20);
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