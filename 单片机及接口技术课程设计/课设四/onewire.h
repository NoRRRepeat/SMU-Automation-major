#ifndef __ONEWIRE_H
#define __ONEWIRE_H
#include <REGX51.H>

sbit DQ = P1^4;  

float rd_temperature(void);  

#endif
