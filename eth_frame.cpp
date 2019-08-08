#include "eth_frame.h"
unsigned long eth_frame::getcrc()
{
                // Возможно не правильно +, может нужно делать или |
    return str.crc[3] + (str.crc[2] << 8) + (str.crc[1] << 16) + (str.crc[0] << 24);
};
int eth_frame::getethtype()
{
    return str.type[1] + (str.type[0] << 8);
};
int* eth_frame::getmacd()
{
    return str.d;
};
int* eth_frame::getmacs()
{
    return str.s;
};
int eth_frame::incapeth(unsigned char *buffer, int lenrecvpack)
{
    if (lenrecvpack <= 0) return -1;
    int i=0;
    lendata=0;
    for (;i<LENMAC; i++) str.d[i]=buffer[i];
    for (;i<LENMAC*2; i++) str.s[i-LENMAC]=buffer[i];
    for (;i<LENMAC*2+LENETHTYPE; i++) str.type[i-LENMAC*2]=buffer[i];
    for (;i<lenrecvpack-LENCRC; i++) data[i-14]=buffer[i];
    lendata = i-14;
    str.crc[0]=buffer[i]; str.crc[1]=buffer[++i]; str.crc[2]=buffer[++i]; str.crc[3]=buffer[++i];
    return lenrecvpack-i;		//Должно быть равно нулю
};
