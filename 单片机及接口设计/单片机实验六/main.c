#include "main.h"
sfr AUXR=0x8e;
u8 channel=0x03;//默认测量通道是AIN3
u16 cnt;//定时器0计数
u8 ad;
u16 analog;
unsigned char urdat=0;
bit ad_mea;
u8 real_T,real_T1,real_T2;

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

void sys_init()
{
   P2 = (P2&0x1f)|0x80;
	 P0=0xff;
	 P2 = (P2&0x1f);
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
	TH0 = (65536 - 5000) / 256;
	TL0 = (65536 - 5000) % 256;
	
	ET0 = 1;
	EA = 1;
	TR0 = 1;
}


void main(){
	sys_init();
	InitTimer0();
	UartInit();
	
	while(1)
		{
			if(ad_mea)
			{
				///////////////////
          SBUF = ' ' ; 
					while(!TI); 
					TI = 0;
					SBUF = real_T + '0'; 
					while(!TI); 
					TI = 0; 
					SBUF = '.'; 
					while(!TI); 
					TI = 0; 
					SBUF = real_T1+'0'; 
					while(!TI); 
					TI = 0; 
					SBUF = real_T2 + '0'; 
					while(!TI); 
					TI = 0; 	
	/////////////////////////////
				ad_mea=0;
				ad = PCF8591_ADin(channel);
				analog = (int)(ad/255.0*5*100);//方便在数码管上显示模拟电压值
        real_T=analog/100;
				real_T1=analog/10%10;
				real_T2=analog%10;
			}
		 		Seg_Set(16,16,16,16,16,analog/100+32,analog/10%10,analog%10);
			  Seg_Show();
				PCF8591_DAout(ad);//将读取的电压值输出到OUT口
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
	}

}


void Timer0_Int() interrupt 1
	{
	TH0 = (65536 - 5000) / 256;  //5ms
	TL0 = (65536 - 5000) % 256;
	cnt++;
	//if(cnt%200==0) read_flag=1;
	if(cnt%200==0) ad_mea=1;//每隔1000ms测一次电压值
	cnt%=1000;
}