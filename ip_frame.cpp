#include "ip_frame.h"

int ip_frame::getlendata()
{
    return lendata;
};
char* ip_frame::getipdestin()
{
    sprintf(head.c.ipd, "%d.%d.%d.%d", head.d.ipd[0], head.d.ipd[1], head.d.ipd[2], head.d.ipd[3]);
    return head.c.ipd;
};
char* ip_frame::getipsource()
{
    sprintf(head.c.ips, "%d.%d.%d.%d", head.d.ips[0], head.d.ips[1], head.d.ips[2], head.d.ips[3]);
    return head.c.ips;
};
char* ip_frame::getcontrolsum()
{
    sprintf(head.c.controlsum, "%03d%03d", head.d.controlsum[0], head.d.controlsum[1]);
    return head.c.controlsum;
};
int ip_frame::getprot()
{
    return head.d.prot;
};
char* ip_frame::gettimeTTL()
{
    sprintf(head.c.timeTTL, "%03d", head.d.timeTTL);
    return head.c.timeTTL;
};
char* ip_frame::getflags()
{
    sprintf(head.c.flags, "%03X", head.d.flags);
    return head.c.flags;
};
char* ip_frame::getsfrag()
{
    sprintf(head.c.sfrag, "%03d%03d", head.d.sfrag[0], head.d.sfrag[1]);
    return head.c.sfrag;
};
char* ip_frame::getidentif()
{
    sprintf(head.c.identif, "%03d%03d", head.d.identif[0], head.d.identif[1]);
    return head.c.identif;
};
char* ip_frame::getsizepack()
{
    sprintf(head.c.sizepack, "%03d%03d", head.d.sizepack[0], head.d.sizepack[1]);
    return head.c.sizepack;
};
char* ip_frame::getservis()
{
    sprintf(head.c.servis,"%03X", head.d.servis);
    return head.c.servis;
};
char* ip_frame::getsizehead()
{
    sprintf(head.c.sizehead, "%X", head.d.sizehead);
    return head.c.sizehead;
};
char* ip_frame::getversion()
{
    sprintf(head.c.IPv,"%X", head.d.IPv);
    return head.c.IPv;
};
int ip_frame::incapip(int *buffer, int lenframe)
{
    unsigned char byte;
    byte = buffer[0];
    head.d.IPv = byte >> 4;
    byte = buffer[0];
    //byte <<= 4;
    head.d.sizehead = buffer[0] & 15;
    if (head.d.sizehead>5) bugs++ ;  //дописать
    head.d.servis = buffer[1];
    head.d.sizepack[0] = buffer[2];
    head.d.sizepack[1] = buffer[3];
    head.d.identif[0] = buffer[4];
    head.d.identif[1] = buffer[5];
    byte = buffer[6];
    head.d.flags = byte >> 5;
    byte = buffer[6];
    byte <<= 3;
    head.d.sfrag[0] = byte >> 3;
    head.d.sfrag[1] = buffer[7];
    head.d.timeTTL = buffer[8];
    head.d.prot = buffer[9];
    head.d.controlsum[0] = buffer[10];
    head.d.controlsum[1] = buffer[11];
    for (int i=0; i<4; i++) {
        head.d.ips[i] = buffer[12+i];
        head.d.ipd[i] = buffer[16+i];
    };
    // Отнимаем длину заголовка и заводим в ответ lendata
    for (lendata=0; lendata<lenframe-(head.d.sizehead*4); lendata++) data[lendata] = buffer[lendata+(head.d.sizehead*4)];
    return 0;
};
