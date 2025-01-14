#include <REGX51.H>
#include <intrins.h>
#include "iic.h"

unsigned char num,count,duty_count,duty,pwm_count;
unsigned char mode=1;  //控制模式
unsigned char number; //led模式
unsigned int interval=400;
unsigned int ref_ad,led_state=0xff;
unsigned char i;  //循环用                 //a 10 b 11 c 12 d 13 e 14 f 15
unsigned int ledarr1[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};   //0111 1111  1011 1111  1101 1111  1110 1111 1111 0111
unsigned int ledarr2[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};    //1111 1110
unsigned int ledarr3[]={0x7e,0xbd,0xdb,0xe7};  //0111 1110 -> 1110 0111
unsigned int ledarr4[]={0xe7,0xdb,0xbd,0x7e};
float ad;
bit key_flag,led_running=1,ad_mea;

sbit S7 = P3 ^ 0; // S7 
sbit S6 = P3 ^ 1; // S6 
sbit S5 = P3 ^ 2; // S5 
sbit S4 = P3 ^ 3; // S4

void led_left_to_right();
void led_right_to_left();
void led_to_middle();
void middle_to_out();
void LED_show();
void LED_set();
void interval_set();
void LED_real_show(unsigned char number);

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

void Sys_Init(){
	P0=0xff;P2=(P2 & 0x1f) | 0x80;P2=(P2 & 0x1f);
	P0=0xaf;P2=(P2 & 0x1f) | 0xa0;P2=(P2 & 0x1f);
	P0=0xff;P2=(P2 & 0x1f) | 0xc0;P2=(P2 & 0x1f); //段码
	P0=0xff;P2=(P2 & 0x1f) | 0xe0;P2=(P2 & 0x1f); //位码
}


void Timer0Init(void)		//1ms 12mhz
{
	TMOD &= 0xF0;		
	TL0 = 0x18;		
	TH0 = 0xFC;	
	TF0 = 0;		
	TR0 = 1;		
	ET0 = 1;
	EA = 1;
}

unsigned char key_scan()
{
    static unsigned char num = 0; 
    unsigned char current_key = 0;     


    if (S7 == 0) current_key = 7;
    else if (S6 == 0) current_key = 6;
    else if (S5 == 0) current_key = 5;
    else if (S4 == 0) current_key = 4;


    if (current_key != num)  //更新
    {
        Delay(20);  
        if (current_key != num)  
        {
            num = current_key;   
            if (current_key != 0)     
            {
                while (current_key == num)  
                {
                    if (S7 == 0) current_key = 7;
                    else if (S6 == 0) current_key = 6;
                    else if (S5 == 0) current_key = 5;
                    else if (S4 == 0) current_key = 4;
                    else current_key = 0;  
                }
                return num;  
            }
        }
    }

    return 0;  
}

unsigned char code duan[] = {                        
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E,
// black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0xFF, 0xBF, 0x89, 0xE1, 0x8F, 0xC7, 0xC8, 0xA3, 0x8C, 0xC1, 0x87, 0xC2, 0x98, 0xAF, 0xC8, 0x91,
    0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0xB9
};    // 0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1


unsigned char code wei[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码
unsigned char seg[8] = {16,16,16,16,16,16,16,16};


void Seg_Show(){
	static unsigned char num;
	
	P0=0xff;P2=(P2 & 0x1f) | 0xe0;P2=(P2 & 0x1f);
	P0=wei[num];P2=(P2 & 0x1f) | 0xc0;P2=(P2 & 0x1f);
	P0=duan[seg[num]];P2=(P2 & 0x1f) | 0xe0;P2=(P2 & 0x1f);
	
	num++;
	num%=8;
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
  Sys_Init();
	Timer0Init();
	while(1)
	{
	   if(key_flag)
		{
			 key_flag=0;
		 	 num = key_scan();
		}
		
		if(ad_mea)
		{
			ad_mea=0;
		  ad = PCF8591_ADin(0x03);    //获取pcf8591电压
			ref_ad = (ad/255.0)*(5.0);  //转化成0-5v   ad/255.0 乘 参考电压（5v）
			if(ref_ad<1.25)
			{
			  duty=90;                  //占空比
				duty_count=1;
			}else if(ref_ad<2.5)
			{
			  duty=50;
				duty_count=2;
			}else if(ref_ad<3.75)
			{
			  duty=10;
				duty_count=3;
			}else{
			  duty=0;
				duty_count=4;
			}
		}
		
	switch(mode)
		{
		  case 1:LED_show();break;
			case 2:LED_set();break;
		  case 3:interval_set();break;
		}
	}
 
}


void LED_show()
{
  if(num==4)
	{
	  mode=2;
		num=0;
	}
//	
//  if(num==5)  //led停止或启动
//	{
//	  led_running = !led_running; 
//		num=0;
//	}
  if(led_running)
	{
	  LED_real_show(number);
	}else{
	  P2=(P2 & 0x1f) | 0x80;
		P0=0xff;
	}
	if(S7==0)
	{
		Delay(10);
	  Seg_Set(17,duty_count,16,16,16,16,16,16);
		while(S7==0);
	}else{
	  Seg_Set(16,16,16,16,16,16,16,16);
	}
}

void LED_set()
{
	if(num==4)
	{
	  mode=3;
		num=0;
	}
  if(num==7)  //加
	{
		number++;   //模式
		if(number>4)
		{
		  number=1;
		}
		num=0;
	}
	
	 if(num==6)  //减
	{
		number--;   //模式
		if(number<1)
		{
		  number=4;
		}
		num=0;
	}
/////////////////////闪烁///////////////
	if(interval<1000)
	{
	 Seg_Set(17,16,17,interval/100%10,interval/10%10,interval%10,16,16);
	 Delay(800);
	 Seg_Set(17,number,17,interval/100%10,interval/10%10,interval%10,16,16);
	 Delay(800);
	}else{
		Seg_Set(17,16,17,interval/1000,interval/100%10,interval/10%10,interval%10,16);
		Delay(800);
	  Seg_Set(17,number,17,interval/1000,interval/100%10,interval/10%10,interval%10,16);
		Delay(800);
	}
	//////////////////////////////////////////
}

void interval_set()
{
  if(num==7)
	{
		num=0;
	  interval+=100;
		if(interval>1200)
		{
		  interval=400;
		}
	}
	
	 if(num==6)
	{
		num=0;
	  interval-=100;
		if(interval<400)
		{
		  interval=1200;
		}
	}
	
	if(num==4)
	{
		num=0;
	  mode=1;
	}
	////////////////////////////////////////////////////////////
	Seg_Set(17,number,17,16,16,16,16,16);
	Delay(800);
	if(interval>=1000)
	{
	Seg_Set(17,number,17,interval/1000,interval/100%10,interval/10%10,interval%10,16);
		Delay(800);
	}else{
	 Seg_Set(17,number,17,interval/100,interval/10%10,interval%10,16,16);
		Delay(800);
	}
///////////////////////////////////////////////////////////////////
}

void LED_real_show(unsigned char number)
{
  switch (number)
	{
	  case 1:led_left_to_right();break;
		case 2:led_right_to_left();break;
		case 3:led_to_middle();break;
		case 4:middle_to_out();break;
		default:break;
	}
}

void led_left_to_right()
{		
	P2=(P2 & 0x1f) | 0x80;
   for(i=0;i<=7;i++)
  {
//    P2=(P2 & 0x1f) | 0x80;
	  led_state=ledarr1[i];
	  Delay(interval);    //waring :因为delay造成的硬性延迟导致了num=5时无法即刻停止
	}                     //waring :因为delay造成的硬性延迟导致了num=5时无法即刻停止
}

void led_right_to_left()
{
	P2=(P2 & 0x1f) | 0x80;
   for(i=0;i<=7;i++)
  {
//		P2=(P2 & 0x1f) | 0x80;
	  led_state=ledarr2[i];
		Delay(interval);
	}
}

void led_to_middle()
{
	P2=(P2 & 0x1f) | 0x80;
 for(i=0;i<=3;i++)
 {
	  led_state=ledarr3[i];
		Delay(interval);
 }

}

void middle_to_out()
{
	P2=(P2 & 0x1f) | 0x80;
  for(i=0;i<=3;i++)
 {
	  led_state=ledarr4[i];
		Delay(interval);
 }

}

void Timer0server() interrupt 1
{
	Seg_Show();
	count++;
	pwm_count++;
	
	    if (pwm_count >= 100) 
    {
        pwm_count = 0;
    }

    if (pwm_count < duty) 
    {
			P2=(P2 & 0x1f) | 0x80;
        P0 = led_state; 
    }
    else
    {
			P2=(P2 & 0x1f) | 0x80;
        P0 = 0xFF; 
    }
		
	if(count%10==0)
	{
	  key_flag=1;
	}
  if(count%50==0)
	{
	 ad_mea=1;
	}
	count%=1000;  //防溢出
	
	
	  if(num==5)  //led停止或启动  因为delay造成的延迟无法立刻生效，放在中断也不会改善
	{
	  led_running = !led_running; 
		num=0;
	}
}