#include "udp_frame.h"
unsigned int udp::getchecksum()
{
    pack.l.checksum = (pack.d.checksum[0] << 8) + pack.d.checksum[1];  // Собираем число из двух байтов
    return pack.l.checksum;
};
unsigned int udp::getports()
{
    pack.l.ports = (pack.d.ports[0] << 8) + pack.d.ports[1];  // Собираем число из двух байтов
    return pack.l.ports;
};
unsigned int udp::getportd()
{
    pack.l.portd = (pack.d.portd[0] << 8) + pack.d.portd[1];  // Собираем число из двух байтов
    return pack.l.portd;
};
int udp::udpincap(int *buffer, int lenframe)
{
    pack.d.ports[0] = buffer[0];
    pack.d.ports[1] = buffer[1];
    pack.d.portd[0] = buffer[2];
    pack.d.portd[1] = buffer[3];
    pack.d.lendata[0] = buffer[4];
    pack.d.lendata[1] = buffer[5];
    pack.d.checksum[0] = buffer[6];
    pack.d.checksum[1] = buffer[7];
    for (lendata=0; (lendata+8) < lenframe; lendata++)
        data[lendata] = buffer[lendata+8];
    return 0;
};
