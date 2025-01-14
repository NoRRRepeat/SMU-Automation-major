#include <REGX51.H>
#include "csb.h"

unsigned char num;  //按键值
unsigned int count; //中断值
unsigned char distance;  //超声波距离
unsigned char mode=1; //模式
unsigned char temp_distance[10]={0,0,0,0,0,0,0,0,0,0};
char i=-1,n=-1;  //数据位   n映射i，作用一样
unsigned char j;
unsigned int processed_data;
unsigned char mangqu=10;
bit key_flag,csb_measure_flag,cao_zuo;
///////////////////led//////////////////////////
bit led1_blink_flag=0,led1_state=0;
unsigned char led1_blink_count=0;
////////////////////led/////////////////////////

sbit S7 = P3 ^ 0; // S7 
sbit S6 = P3 ^ 1; // S6 
sbit S5 = P3 ^ 2; // S5 
sbit S4 = P3 ^ 3; // S4

void ce_ju_show();
void shu_ju_hui_show();
void can_shu_set();

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

unsigned char key_scan()  //获取键盘值
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
	csb_timer_init();
	while(1)
	{
	   if(key_flag)
		{
			 key_flag=0;
		 	 num = key_scan();
		}
	   if(csb_measure_flag)
		 {
		   csb_measure_flag=0;
			 distance = csb_measure();
		 }
	  switch (mode)
		{
		  case 1: ce_ju_show();break;
			case 2: shu_ju_hui_show();break;
			case 3: can_shu_set();break;
		}
		
	}
}

void ce_ju_show()
{
	
	P2=(P2 & 0x1f) | 0x80;   //mode2 回显模式回来之后 ，把灯全关
	P0=0xff;
	
	if(num==6)
	{
	  mode=3;
		num=0;
	}
	
	if(num==5)
	{
	  mode=2;
		num=0;
		n=-1;
	}
	
  if(num==7)
	{
		cao_zuo=~cao_zuo;
	  num=0;
	}
  if(num==4)
	{
		
		//////////////////////
      led1_blink_flag = 1;   // LED
      led1_blink_count = 0;  // 
		/////////////////////////
		
		num=0;
		i++;                       // i=-1 ---》i++ 
		if(distance>255-mangqu)
		{
		  distance=255-mangqu;
		}
		if(distance<mangqu)
		{
		  distance=mangqu;
		}                           //  i从-1开始
	  temp_distance[i]=distance;  //按下一次 temp[0]=distance i=0再按一次 temp[1]=distance i=1
		i=i%10;
	}
	
	
	if(cao_zuo==0)
	{
	  processed_data=temp_distance[i-1]; //上一次数据
	}else{
	  processed_data=temp_distance[i]+temp_distance[i-1];  //本次+上次
	}
	if(i>=0)
	{
   Seg_Set(cao_zuo,16,processed_data/100,processed_data/10%10,processed_data%10+32,temp_distance[i]/100,temp_distance[i]/10%10,temp_distance[i]%10);
	}else{
	 Seg_Set(cao_zuo,16,0,0,32,distance/100,distance/10%10,distance%10);
	}
}

void shu_ju_hui_show()
{	
	P2=(P2 & 0x1f) | 0x80;   //灯全亮
	P0=0x00;
	P2=(P2 & 0x1f);
	
  if(num==5)
	{
	  mode=1;  //返回测距模式
		num=0;
	}
  if(num==7)  //回显+1
	{
		num=0;
	  n++;
		n=n%10;
	}
	if(n>=0)
	{
	Seg_Set(n/10,n%10,16,16,16,temp_distance[n]/100,temp_distance[n]/10%10,temp_distance[n]%10);  //数据回显
	}else{
	Seg_Set(0,0,0,0,0,0,0,0);   //n初始为-1  开始时数码管全0
	}
}
	
void can_shu_set()
{
	P2=(P2 & 0x1f) | 0x80;   //灯全亮
	P0=0x00;
	P2=(P2 & 0x1f);
	if(num==6)
	{
	  mode=1;
		num=0;
	}
  if(num==7)
	{
	  mangqu+=10;
		if(mangqu>90)
		{
		  mangqu=0;
		}
	}
  Seg_Set(15,16,16,16,16,16,mangqu/10,mangqu%10);  //F  +mangqu
}


void timer0server() interrupt 1
{
	 Seg_Show();
   count++;
	 if(count%10==0)
	 {
	    key_flag=1;
	 }
	 if(count%50==0)
	 {
	   csb_measure_flag=1;  //超声波测量
		 ////////////////////////////////////////
		      if (led1_blink_flag)
        {
            led1_state = ~led1_state; 
            P2 = (P2 & 0x1f) | 0x80; 
            P0 = led1_state ? 0xFE : 0xFF; 

            if (led1_blink_count >= 20) 
            {
                led1_blink_flag = 0;    
                led1_blink_count = 0;   
                P0 = 0xFF;              
            }
            else
            {
                led1_blink_count++; 
            }
						
        }
		//////////////////////////////////////////////////////
	 }
	 
	 count%=1000;
}

