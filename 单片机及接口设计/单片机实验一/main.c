#include <REGX51.H>
#include "Delay.h"

unsigned char count;
sbit S7 = P3 ^ 0; //  S7
sbit S6 = P3 ^ 1; 
sbit BEEP = P0^6;
bit flash_flag = 0;
void SelectHC138(unsigned char channel)
{
    switch (channel)
    {
    case 4:
        P2 = (P2 & 0X1F) | 0X80; 
        break;
    case 5:
        P2 = (P2 & 0X1F) | 0Xa0; 
        break;
    case 6:
        P2 = (P2 & 0X1F) | 0Xc0; 
        break;
    case 7:
        P2 = (P2 & 0X1F) | 0Xe0; 
        break;
    }
}

void sys_init()
{
  SelectHC138(4);
	P0=0xff;
	SelectHC138(5);
	BEEP=0;
}

void buzz_flash_2s()
{
	 if(flash_flag)
	 {
        SelectHC138(4);
        P0 = 0x00; 
        SelectHC138(5);
        BEEP = 1;  
        while (count < 20); 
        count = 0;
        SelectHC138(4);
        P0 = 0xFF; 
        SelectHC138(5);
        BEEP = 0;  
        while (count < 20); 
        count = 0;
	 }
}

void InitTimer0()   // **************************定时器初始化*****************************************
{
	TMOD = 0X01;
	TH0 = (65535 - 50000) / 256;                 //           50ms
	TL0 = (65535 - 50000) % 256;
	
	ET0 = 1;
	EA = 1;                                 //总中断开
	TR0 = 1;
}


void main()
{
  sys_init();
	InitTimer0(); 
  while(1)
	{
	  Delay(10);
		if(S7==0)
		{
		   Delay(10);
			 if(S7==0)
			 {
			   flash_flag=1;
			 }
		 }
		if(S6==0)
			{
			  Delay(10);
				if(S6==0)
				{
					sys_init();
					flash_flag=0;
				}
			}		
  
	 buzz_flash_2s();	 
	}
}



void serverTimer0() interrupt 1      //中断服务子程序
{
  TH0 = (65535 - 50000) / 256;   
	TL0 = (65535 - 50000) % 256;
  count++;                         //每50ms count++
}