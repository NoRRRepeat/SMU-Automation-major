#include <reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;
uchar lednum;
uchar led_state;
sfr P4 = 0XC0;

sbit R1 = P3^0;
sbit R2 = P3^1;
sbit R3 = P3^2;
sbit R4 = P3^3;

sbit C4 = P3^4;
sbit C3 = P3^5;
sbit C2 = P4^2; //P36 -> P42
sbit C1 = P4^4; //P37 -> P44



void SelectHC138(uchar channel)
{
	switch(channel)
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

void outerinterrupt_init()
{
  EA=1;
	EX0=1;
	IT0=1;
	EX1=1;
	IT1=1;
	
}


void Initsys()
{
	SelectHC138(5);
	P0 = 0X00;
	SelectHC138(4);
	P0 = 0XFF;
}

void selectled()
{
    P2 = (P2 & 0x1F) | 0x80;  
    P0 =lednum;      
}

void selectled_2()
{
   P2 = (P2 & 0x1F) | 0x80;
	 P0=(lednum &led_state);
}

void ScanKeyMulti()
{
	R1 = 0;
	R2 = R3 = R4 = 1;
	C1 = C2 = C3 = C4 = 1;
	if(C1 == 0)//0
	{
		lednum=0xff;          //1111 1111
		selectled();
		while(C1 == 0);
	}
	else if(C2 == 0)
	{
		lednum=0x7f;             //0111 1111
		selectled();
		while(C2 == 0);
	}
	else if(C3 == 0)
	{
	 lednum=0xbf;            //1011 1111
		selectled();
		while(C3 == 0);
	}
	else if(C4 == 0)//3
	{
	 lednum=0x3f;            //0011 1111
		selectled();
		while(C4 == 0);
	}
	R2 = 0;
	R1 = R3 = R4 = 1;
	C1 = C2 = C3 = C4 = 1;
	if(C1 == 0)//4
	{
		lednum=0xdf;                //1101 1111
		selectled();
		while(C1 == 0);
	}
	else if(C2 == 0)
	{
		lednum=0x5f;             //0101 1111
		selectled();
		while(C2 == 0);
	}
	else if(C3 == 0)
	{
		lednum=0x9f;            //1001 1111
		selectled();
		while(C3 == 0);
	}
	else if(C4 == 0)//7
	{
		lednum=0x1f;           //0001 1111
		selectled();
		while(C4 == 0);
	}
	R3 = 0;
	R2 = R1 = R4 = 1;
	C1 = C2 = C3 = C4 = 1;
	if(C1 == 0)  //8
	{
		lednum=0xef;            //1110 1111
		selectled_2();
		while(C1 == 0);
	}
	else if(C2 == 0)
	{
		 lednum=0x6f;        //0110 1111
		selectled_2();
		while(C2 == 0);
	}
	else if(C3 == 0)  //10
	{
		lednum=0xaf;            //1010 1111
		selectled_2();
		while(C3 == 0);
	}
	else if(C4 == 0)
	{
		lednum=0x2f;        //0010 1111
		selectled_2();
		while(C4 == 0);
	}

	R4 = 0;
	R2 = R3 = R1 = 1;
	C1 = C2 = C3 = C4 = 1;
	if(C1 == 0) //12
	{
		lednum=0xcf;   //1100 1111
		selectled_2();
		while(C1 == 0);
	}
	else if(C2 == 0)
	{
	  lednum=0x4f;            //0100 1111
		selectled_2();
		while(C2 == 0);
	}
	else if(C3 == 0)  //14
	{
		lednum=0x8f;           //1000 1111
		selectled_2();
		while(C3 == 0);
	}
	else if(C4 == 0)  //15
	{
		lednum=0x0f;   //0000 1111
		selectled_2();
		while(C4 == 0);
	}
	
}

void main()
{
	Initsys();
	outerinterrupt_init();
	while(1)
	{
		ScanKeyMulti();
	}
}

void key_32(void) interrupt 0
{
    EX0 = 0;  

    if (R3 == 0)  
    {
        led_state = 0xFC;  
    }

    EX0 = 1; 
}

void key_33(void) interrupt 2
{
    EX1 = 0;  

    if (R4 == 0)  
    {
        led_state = 0xF3; 
    }

    EX1 = 1; 
}