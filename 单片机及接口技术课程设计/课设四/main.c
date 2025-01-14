#include <REGX51.H>
#include "iic.h"
#include "onewire.h"

unsigned char index=0;  //��ѹ-Ƶ��-�¶� ģʽ
unsigned char mode=1;
unsigned char num; //����ֵ
unsigned char count; //�ж��ۼ�ֵ
unsigned char ledcount;
unsigned int final_freq,save_freq;
unsigned int final_ad,save_ad;        //����0-5v��ѹת��Ϊ��λ��,����ָ��λʮλ��λ
unsigned int final_T,save_temp;
unsigned int processed_threshold=2;  //0.2*10
float T,ad,threshold_ad=0.2;  //ֱ�Ӷ�ȡ�����¶�ֵ
sfr AUXR =0x8E;
bit key_flag,freq_flag,temperature_flag,ad_flag,voltage_flag=0;

sbit S7 = P3 ^ 0; // S7 
sbit S6 = P3 ^ 1; // S6 
sbit S5 = P3 ^ 2; // S5 
sbit S4 = P3 ^ 3; // S4

void shu_ju_show();
void shu_ju_hui_xian();
void voltage_set();

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
	P0=0xff;P2=(P2 & 0x1f) | 0xc0;P2=(P2 & 0x1f); //����
	P0=0xff;P2=(P2 & 0x1f) | 0xe0;P2=(P2 & 0x1f); //λ��
}


void Timer1Init(void)		//1ms 12mhz
{
	AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ	
	TL1 = 0x18;		
	TH1 = 0xFC;	
	TF1 = 0;		
	TR1 = 1;		
	ET1 = 1;
	EA = 1;
}

unsigned char key_scan()  //��ȡ����ֵ
{
    static unsigned char num = 0; 
    unsigned char current_key = 0;     


    if (S7 == 0) current_key = 7;
    else if (S6 == 0) current_key = 6;
    else if (S5 == 0) current_key = 5;
    else if (S4 == 0) current_key = 4;


    if (current_key != num)  //����
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
// black  -     H      J     K     L    N     o     P    U     t      G      Q     r    M     y
    0xFF, 0xBF, 0x89, 0xE1, 0x8F, 0xC7, 0xC8, 0xA3, 0x8C, 0xC1, 0x87, 0xC2, 0x98, 0xAF, 0xC8, 0x91,
    0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0xB9
};    // 0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1


unsigned char code wei[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //λ��
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

void NE555_Init(){
  AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD = 0x05;		//���ö�ʱ��ģʽ
	TL0 = 0;		//���ö�ʱ��ֵ
	TH0 = 0;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
}

void main()
{
  Sys_Init();
	NE555_Init();
	Timer1Init();
  while(1)
	{
		
	  if(key_flag)
		{
			 key_flag=0;
		 	 num = key_scan();
		}
////////////////////////////////		
		if(freq_flag)
		{
			freq_flag=0;
			TR0 = 0;
			final_freq=TH0*256+TL0;
			TH0=0;
			TL0=0;
			TR0=1;
		}
/////////////////////////////////////
		if(temperature_flag)
		{
		  temperature_flag=0;
			
			ET0=0;//�ر��жϣ���ֹӰ��ʱ��
			T = rd_temperature();//��ȡ�¶�ֵ
			ET0=1;//��ȡ�����������ж�
		  final_T=(int)(T*100);
		}
////////////////////////////////////////////
		if(ad_flag)
		{
		  ad_flag=0;
			ad = PCF8591_ADin(0x03);    //��ȡpcf8591��ѹ
			final_ad = (int)(ad/255.0*5*100);;  //ת����0-5v   ad/255.0 �� �ο���ѹ��5v��
		  
		}
		
		switch (mode)
		{
		  case 1: shu_ju_show();break;
			case 2: shu_ju_hui_xian();break;
			case 3: voltage_set();break;
		}
//		Seg_Set(25,16,16,16,16,16,final_ad/100+32,final_ad/10%10);
//		Seg_Set(15,16,final_freq/100000%10,final_freq/10000%10,final_freq/1000%10,final_freq/100%10,final_freq/10%10,final_freq%10);
//    Seg_Set(12,16,16,16,final_T/1000%10,final_T/100%10+32,final_T/10%10,final_T%10);
	}

}

void shu_ju_show()
{
	if(num==4)
	{
		num=0;
	  index++;
		index=index%3;   //index=0 ��ѹ index=1 Ƶ�� index=2 �¶� 
	}
	
	if(num==5)
	{
		num=0;
	  save_temp=final_T;
		save_ad=final_ad;
		save_freq=final_freq;
	}
	
  if(num==6)
	{
		index=0;  //����ȥ��index=0
	  num=0;
	  mode=2;  //���ݻ���
	  P2=(P2 & 0x1f) | 0x80;   //��ѹģʽ�� led 1 ��
	  P0=0xff;
		P2=(P2 & 0x1f);
	}
  if(index==0)
	{
		  P2=(P2 & 0x1f) | 0x80;   //��ѹģʽ�� led 1 ��
	    P0=0xfe;
		  P2=(P2 & 0x1f);
	  	Seg_Set(25,16,16,16,16,16,final_ad/100+32,final_ad/10%10);
	}else if(index==1)   //Ƶ��ģʽ�� led 2 ��
	{
		  P2=(P2 & 0x1f) | 0x80;  
	    P0=0xfd;  //1111 1101
		  P2=(P2 & 0x1f);
	   Seg_Set(15,16,final_freq/100000%10,final_freq/10000%10,final_freq/1000%10,final_freq/100%10,final_freq/10%10,final_freq%10);
	}else if(index==2)  //�¶�ģʽ�� led3 ��
    {
			P2=(P2 & 0x1f) | 0x80;  
	    P0=0xfb;   //1111 1011
			P2=(P2 & 0x1f);
		 Seg_Set(12,16,16,16,final_T/1000%10,final_T/100%10+32,final_T/10%10,final_T%10);
		}
	if(final_ad>(threshold_ad*100) && voltage_flag==1)
	{
	  if (ledcount % 2 == 0) 
		{
			P2 = (P2 & 0x1f) | 0x80;  
			P0 = 0x0f;  // L4-L8 
		}else
		{
			P2 = (P2 & 0x1f) | 0x80;  
			P0 = 0xFF;  // L4-L8 
		}
	}
	
}
void shu_ju_hui_xian()
{
  if(num==4)
	{
		num=0;
	  index++;
		index=index%3;
	}
	
	
  if(num==6)
	{
	  num=0;
		mode=3;  //������ʾ
	}

	  if(index==0)
	{
	  	Seg_Set(18,25,16,16,16,16,save_ad/100+32,save_ad/10%10);
	}else if(index==1)
	{
	   Seg_Set(18,15,save_freq/100000%10,save_freq/10000%10,save_freq/1000%10,save_freq/100%10,save_freq/10%10,save_freq%10);
	}else if(index==2)
    {
		 Seg_Set(18,12,16,16,save_temp/1000%10,save_temp/100%10+32,save_temp/10%10,save_temp%10);
		}
  
}



void voltage_set()
{
  if(num==7)
	{
		voltage_flag=1;
		num=0;
	  threshold_ad+=0.2;    //0.2-5.0 v
		if(threshold_ad>5.0)
		{
		  threshold_ad=0.2;
		}
		processed_threshold=(int)(threshold_ad*10);  //ת���������ʾ
	}
  if(num==6)
	{
		index=0;
	  num=0;
		mode=1;    //������ʾ
	}

	Seg_Set(24,16,16,16,16,16,processed_threshold/10+32,processed_threshold%10);
}
void timer1server() interrupt 3
{
  Seg_Show();
  count++;
  if(count%10==0)
	{
	  key_flag=1;
	}

  if(count%50==0)
	{
	  ad_flag=1;
	}
	
	if(count%100==0)   //100ms
	{
	  temperature_flag=1;
	}

	if(count%1000==0)
	{
	  freq_flag=1;
	}
	
	if (count % 200 == 0)  // 200ms
{
	 ledcount++;
}

  count=count%1000;
}