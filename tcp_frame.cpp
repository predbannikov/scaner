#include "tcp_frame.h"
int tcp::getlendata()
{
    return lendata;
};
unsigned int tcp::getportd()
{
    pack.l.portd = (pack.d.portd[0] << 8) + pack.d.portd[1];
    return pack.l.portd;
};
unsigned int tcp::getports()
{
    pack.l.ports = (pack.d.ports[0] << 8) + pack.d.ports[1];
    return pack.l.ports;
};
int tcp::tcpincap(int *buffer, int lenframe)
{
    pack.d.ports[0] = buffer[0];
    pack.d.ports[1] = buffer[1];
    pack.d.portd[0] = buffer[2];
    pack.d.portd[1] = buffer[3];
    pack.d.nposled[0] = buffer[4];
    pack.d.nposled[1] = buffer[5];
    pack.d.nposled[2] = buffer[6];
    pack.d.nposled[3] = buffer[7];
    pack.d.npodtver[0] = buffer[8];
    pack.d.npodtver[1] = buffer[9];
    pack.d.npodtver[2] = buffer[10];
    pack.d.npodtver[3] = buffer[11];
    char tmp = buffer[12];
    tmp >>= 4;
    pack.d.scdata = 4;
    // добавить поле зарезервированное
    tmp = buffer [13];
    tmp <<= 2;
    pack.d.flags = tmp >> 2;
    pack.d.sizewindow[0] = buffer[14];
    pack.d.sizewindow[1] = buffer[15];
    pack.d.checksum[0] = buffer[16];
    pack.d.checksum[1] = buffer[17];
    pack.d.point[0] = buffer[18];
    pack.d.point[1] = buffer[19];
    //добавить проверку на использование этих опций
    pack.d.option[0] = buffer[20];
    pack.d.option[1] = buffer[21];
    pack.d.option[2] = buffer[22];
    pack.d.option[3] = buffer[23];
    for (lendata=24; lendata < lenframe; lendata++)
        data[lendata-24] = buffer[lendata];
    return 0;
};
