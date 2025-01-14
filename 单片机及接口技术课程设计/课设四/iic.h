#ifndef _IIC_H
#define _IIC_H

#include <REGX51.H>
#include "intrins.h"

sbit SDA = P2^1;
sbit SCL = P2^0;

void IIC_Start(void); 
void IIC_Stop(void);  
bit IIC_WaitAck(void);  
void IIC_SendAck(bit ackbit); 
void IIC_SendByte(unsigned char byt); 
unsigned char IIC_RecByte(void); 
void PCF8591_DAout(unsigned char Data);
unsigned char PCF8591_ADin(unsigned char addr);
#endif