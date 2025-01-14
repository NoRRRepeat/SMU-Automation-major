#include <REGX51.H>

sbit Beep = P0 ^ 6;

unsigned char duty; //ռ�ձ�

//(466Hz-3517Hz)
code unsigned int FreqTable[] = {
    466, 523, 587, 659, 698, 784, 880,          // ����1-7
    942, 1047, 1175, 1319, 1397, 1568, 1760,    // ����1-7  //932-960
    1865, 2094, 2351, 2633, 2792, 3134, 3517    // ����1-7
};

//����+���� 
code unsigned char music[] = {
	11,180,  11,180,  10,180,  10,180,    //index
	9,180,   9,180,   8,180,  8,180,
	11,40,   10,40,    11,40,  14,40,
	11,40,   10,40,    11,40,  10,40,
	11,40,   13,40,   11,40,  10,40,
	11,40,   10,40,    11,40,  13,40,
	11,40,   10,40,    10,40,  9,40,
	10,40,   15,40,    10,40,  9,40,
	13,40,   9,40,     10,40,  15,40,
	10,40,   9,40,     10,40,  9,40,
	10,40,   15,40,    10,40,   9,40,
	13,40,   9,40,     10,40,   15,40,
	10,40,   9,40,  
	10,40,   11,40,    10,40,  14,40,  10,160,
	10,80,   10,40,    11,40,  10,40,  11,40,  13,40,  11,40,
	10,40,   11,40,    10,40,  6,40,   9,160,
	9,80,    17,80,    15,40,  13,40,  11,80,
	10,40,   11,40,    10,40,  6,40,   10,160,
	10,80,   10,40,    11,40,  10,40,  11,40,
	13,40,   11,40,    10,40,  11,40,  10,40,
	9,40,    6,160,    6,20,   7,20,   9,20,
	9,20,    10,20,    11,20,  12,20,  13,20,
	14,20,    15,20,   17,20,   18,20,
	

    0xFF   
};

unsigned char beat, timer0High, timer0Low;   // beat ����  timer0H��ʱ����λ
unsigned char index =0;  //����λʶ��
unsigned char note;                          //����


void Sys_Init(){
	P0=0xff;P2=(P2 & 0x1f) | 0x80;P2=(P2 & 0x1f);
	P0=0xaf;P2=(P2 & 0x1f) | 0xa0;P2=(P2 & 0x1f);
	P0=0xff;P2=(P2 & 0x1f) | 0xc0;P2=(P2 & 0x1f); //����
	P0=0xff;P2=(P2 & 0x1f) | 0xe0;P2=(P2 & 0x1f); //λ��
}


void SetTimer(unsigned int freq) {
    unsigned int TimerValue = 65536 - (12000000 / 12 / 2 / freq);         // 120000000/12  12mhz/12��ʱ��Ƶ�� 1mhz   -��  1mhz/2 ���ɷ��������跭ת����    /freq�ó������ڼ���ֵ
    timer0High = TimerValue/256;   //��ȡ�߰�λ                       //���� 12mhz/12/2/466 =1073  timervalue=65536-1073=64463   
    timer0Low = TimerValue%256;    //��ȡ�Ͱ�λ
}


void Playmusic() {
    if (music[index] == 0xFF) 
			   {                           //ʶ��ĩβ0xff����ͷ��ʼ����
          index = 0;  
         }
				 else 
			      {
								note = music[index++];   //������
								beat = music[index++];   //�ٽ���
							
							  // led
                P2 = (P2 & 0x1F) | 0x80;
                P0 = ~(0x01 << (note % 7));  //��Ϊ0xfe,��<<�����
							  P2 = (P2 & 0x1F);
//							switch (note%7)
//							{
//								case 0: duty=10; break;
//								case 1: duty=20;break;
//								case 2: duty=30;break;
//								case 3: duty=45;break;
//								case 4: duty=60;break;
//								case 5: duty=75;break;
//								case 6: duty=100;break;
//							}

								SetTimer(FreqTable[note]);  //����ó���ʱ���ߵͺ�Ū��Ƶ�ʼ�����
								TR0 = 1; 
             }
}


void Timer0server() interrupt 1 {

    TR0 = 0;  //�ض�ʱ��
    P2 = (P2 & 0x1F) | 0xA0;  
    Beep = ~Beep;  
    P2 = (P2 & 0x1F);  
    TH0 = timer0High;    //����ʹ��Ƶ�ʴﵽ
    TL0 = timer0Low;
    TR0 = 1;  
}

// Timer1
//void Timer1server() interrupt 3 {
//    static unsigned char pwmCounter = 0;
//    pwmCounter++;

//     	P2 = (P2 & 0x1F) | 0x80;
//    if (pwmCounter <= duty)
//			{
//				
//				   P0=0xfe;
//				
//          //P0 = (P0 & 0x40) | 0xBF;   
//			
//      } else 
//			   {
//					 P0=0xff;
//					  //P0 = (P0 & 0x40) | 0xFF;  
//           //P0=0xff;   
//					  
//         }
//				 P2 = (P2 & 0x1F); 
//    if (pwmCounter >= 100) pwmCounter = 0;  
//}

void DelayMs(unsigned int ms) {
    unsigned int i, j;
    for (i = ms; i > 0; i--) {
        for (j = 123; j > 0; j--);  
    }
}


void main() {
    Sys_Init(); 
    
    TMOD = 0x01;  // tmod=0x01 ��ʱ��0   ////& 0x10��ʱ��1  
    EA = 1;  
    ET0 = 1;  
    
//	 // ��ʱ��1
//	
//    TH1 = 0xFC;  
//    TL1 = 0x18;  
//    TR1 = 1; 
//	  ET1 = 1; 
	
    while (1) {
        Playmusic();  
        DelayMs(beat * 50);    //*50ms
        TR0 = 0;  
    }
}
