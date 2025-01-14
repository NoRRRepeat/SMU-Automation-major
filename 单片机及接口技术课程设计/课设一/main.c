#include <REGX51.H>
#include "ds1302.h"

unsigned char mode=1; //默认时间显示模式 //模式1 时间显示 模式2 时间更改 模式3 闹钟更改
unsigned char num; //按键数值
unsigned char count; //中断值 
unsigned char temp[3];//保存时间设置时变量
unsigned char index=0; //选择时设置位    
unsigned char clock[3];  //保存闹钟设置变量
unsigned char hour,min,sec;
bit key_flag,time_flag,buzz_on_flag;

sbit S7 = P3 ^ 0; // S7 
sbit S6 = P3 ^ 1; // S6 
sbit S5 = P3 ^ 2; // S5 
sbit S4 = P3 ^ 3; // S4

sbit beep = P0^6;
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

//unsigned char key_scan()
//{
//	
//   if(S7==0)
//	 {
//	   Delay(20);
//		  if(S7==0)
//			{
//			  num=7;
//				while(S7==0);
//			}
//	 }else if(S6==0)
//	 {
//	   Delay(20);
//		  if(S6==0)
//			{
//			  num=6;
//				while(S6==0);
//			}
//		}
//	  else if(S5==0)
//	 {
//	   Delay(20);
//		  if(S5==0)
//			{
//			  num=5;
//				while(S5==0);
//			}
//	 }else if(S4==0)
//	 {
//	   Delay(20);
//		  if(S4==0)
//			{
//			  num=4;
//				while(S4==0);
//			}
//	 }else{
//	 return 0;
//	 }
// return num;
//}
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

void Time_set();
void Time_show();
void clock_set();

void main()
{
  Sys_Init();
  Timer0Init();
  DS1302_Settime(23,59,50);
	while(1)
	{
	  
		if(key_flag)
		{
			 key_flag=0;
		 	 num = key_scan();
		}
		
		switch(mode)
		{
		  case 1:Time_show();break;
			case 2:Time_set();break;
			case 3:clock_set();break;
		}
		
	  if(time_flag)
		{
			time_flag=0;
			hour = Read_Ds1302_Byte(0x85);
			min = Read_Ds1302_Byte(0x83);
			sec = Read_Ds1302_Byte(0x81);
			if((hour/16*10+hour%16)==clock[0])  //如果显示时间与 闹钟数组的 小时 分钟 秒 一一对应 就说明要打开闹钟
			{
				if((min/16*10+min%16)==clock[1])
					{
					if((sec/16*10+sec%16)==clock[2])
						{
						    buzz_on_flag=1;
					  }
				  }
			 }	
		}
		if(buzz_on_flag==1)
		{
		  P2=(P2 & 0x1f) | 0x80;
			P0=0x00;
		  P2=(P2 & 0x1f) | 0xa0;
			beep = 1;
			if(num)
			{
			  num=0;
				buzz_on_flag=0;
				P2=(P2 & 0x1f) | 0x80;
				P0=0xff;
			  P2=(P2 & 0x1f) | 0xa0;
				beep = 0;
				P2=(P2 & 0x1f) ;
			}
		 }
		
	}
}

void Time_show()
{
  if(num==4 && buzz_on_flag==0)
	{
	  mode=2; //进入设置模式  ,  将小时分钟秒保存成固定值后再传入time_set
		temp[0]=hour/16*10+hour%16;    //           即 temp
		temp[1]=min/16*10+min%16;
		temp[2]=sec/16*10+sec%16;
		num=0;
	}
	
	if(num==7 && buzz_on_flag==0)  
	{
	  mode=3;  //进入闹钟设置模式
		index=0;
		temp[0]=hour/16*10+hour%16;    
		temp[1]=min/16*10+min%16;
		temp[2]=sec/16*10+sec%16;
		num=0;
	}
  Seg_Set(hour/16,hour%16,17,min/16,min%16,17,sec/16,sec%16);
}

void Time_set()
{
	if(num==4 && buzz_on_flag==0)
	{
	  index++;
		index=index%3;
	}
	if(num==5 && buzz_on_flag==0)  //加1
	{
	    temp[index]++;
			switch(index)
		{
			case 0:if(temp[index]>23)temp[index]=0;break;
			case 1:if(temp[index]>59)temp[index]=0;break;
			case 2:if(temp[index]>59)temp[index]=0;break;
		}
		num=0;
	}
  if(num==6 && buzz_on_flag==0)  //设置完成
	{
	  	ET0=0;
			DS1302_Settime(temp[0],temp[1],temp[2]);
			ET0=1;
			index=0;
		  num=0;
			mode=1; //返回mode1->time_show显示模式
	}
	//////////////////////////////////////////////////////////
		if(sec%16%2){  //0.5s闪烁一次
		Seg_Set(temp[0]/10,temp[0]%10,17,temp[1]/10,temp[1]%10,17,temp[2]/10,temp[2]%10);

	}else if(sec%16%2==0){
		switch(index){
			case 0:Seg_Set(16,16,17,temp[1]/10,temp[1]%10,17,temp[2]/10,temp[2]%10);break;
			case 1:Seg_Set(temp[0]/10,temp[0]%10,17,16,16,17,temp[2]/10,temp[2]%10);break;
			case 2:Seg_Set(temp[0]/10,temp[0]%10,17,temp[1]/10,temp[1]%10,17,16,16);break;
		}
//////////////////////////////////////////////////////////
  }
}

void clock_set()
{
  if(num==4 && buzz_on_flag==0)
	{
	  index++;
		index=index%3;
	}
  	if(num==5 && buzz_on_flag==0)  //加1
	{
	    clock[index]++;
			switch(index)
		{
			case 0:if(clock[index]>23)clock[index]=0;break;
			case 1:if(clock[index]>59)clock[index]=0;break;
			case 2:if(clock[index]>59)clock[index]=0;break;
		}
		num=0;
	}
	if(num==6 && buzz_on_flag==0)
	{
		 ET0=0;
		 DS1302_Settime(temp[0],temp[1],temp[2]);
		 ET0=1;
	   index=0;
		 num=0;
		 mode=1;
	}
	////////////////////////////////////////////////////
   if(sec%16%2){
		Seg_Set(clock[0]/10,clock[0]%10,17,clock[1]/10,clock[1]%10,17,clock[2]/10,clock[2]%10);

	}else if(sec%16%2==0){
		switch(index){
			case 0:Seg_Set(16,16,17,clock[1]/10,clock[1]%10,17,clock[2]/10,clock[2]%10);break;
			case 1:Seg_Set(clock[0]/10,clock[0]%10,17,16,16,17,clock[2]/10,clock[2]%10);break;
			case 2:Seg_Set(clock[0]/10,clock[0]%10,17,clock[1]/10,clock[1]%10,17,16,16);break;
		}
	////////////////////////////////////////////////////////////
 }
}
void Timer0server() interrupt 1
{
	Seg_Show();
  count++;
  if(count%10==0)  //10ms  
	{
	  time_flag=1;
	}
	if(count%50==0)
	{
	  key_flag=1;
	}
}