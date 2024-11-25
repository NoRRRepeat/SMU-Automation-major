#include <REGX51.H> 
#include "Delay.h"
sbit buzz=P0^6;
sbit S4=P3^3;
sbit S5=P3^2;
unsigned int choose_hz=1,control_sound=0,k=0,x=0,y=0;

void outerinit()
{
  EX0=1;
	EX1=1;
}

void InitTimer0() //定时器初始化
{
	TMOD = 0X01;
	TH0 = (65536 - 500) / 256;
	TL0 = (65536 - 500) % 256;
	
	ET0 = 1;
	EA = 1;
	TR0 = 1;
}


void main()
{
	InitTimer0();
	outerinit();
	P2 = (P2 & 0X1F) | 0Xa0;
	buzz = 0X00;
	while(1)
	{	
		if(control_sound==0) buzz=0x00;
		if(choose_hz==1&&control_sound==1) //hz=0 -> 1khz    hz=1 ->500hz
		{
			k=0;
			while(k<1);
			buzz=~buzz;
			x++;
			if(x>=200) 
       {
				 choose_hz=0;  //跳到另一个蜂鸣器
			   x=0;
			 }		
		}
		if(choose_hz==0&&control_sound==1)
		{	
			k=0;
			while(k<2);
				buzz=~buzz;
			  y++;
			if(y>=200) 
      {
				choose_hz=1;  //跳到另一个蜂鸣器
			   y=0;
			}
		}
	}
}
void  ServerTimer0() interrupt 1
{
	TH0 = (65535 - 500) / 256;   //0.5ms
	TL0 = (65535 - 500) % 256;
	k++;
}

void isr0 () interrupt 0
{
	
	Delay(10);
	if(S5==0) 
		{	
		  control_sound=1;  //=1  响
		}
	Delay(10);
	while(S5==0);
}

void isr2 () interrupt 2
{
	Delay(10);
	if(S4==0)
   { 
	   control_sound=0;
	   buzz=0x00;
	 }
	 Delay(10);
	while(S4==0);
}
