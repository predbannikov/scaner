#ifndef		IP_FRAME_H
#define		IP_FRAME_H

#include 	<stdio.h>

#define		LENDATAIP	1500
#define		LENIP		4

struct ipheaddigit{		//структура IP заголовка с целочисленными данными
	int IPv;
	int sizehead;
	//int DSCP;
	//int ECN;
	int servis;
	int sizepack[2];
	int identif[2];
	int flags;
	int sfrag[2];
	int timeTTL;
	int prot;
	int controlsum[2];
	int ips[4];
	int ipd[4];

};
struct ipheadstr{		// структура IP зголовка посимвольно
	char IPv[1];
	char sizehead[1];
	char servis[3];
	char sizepack[6];
	char identif[6];
	char flags[3];
	char sfrag[6];
	char timeTTL[3];
	char prot[3];
	char controlsum[6];
	char ips[15];
	char ipd[15];

};
struct iphead{
	ipheaddigit d;
	ipheadstr c;
};
class ip_frame
{
public:
	//char tmp[100];
	int incapip(int *buffer, int lenframe);  // Сюда входят данные на обработку нашим классом
	int data[1500];
	char* getversion();
	char* getsizehead();
	char* getservis();
	char* getsizepack();
	char* getidentif();
	char* getflags();
	char* getsfrag();
	char* gettimeTTL();
	int getprot();
	char* getcontrolsum();
	char* getipsource();
	char* getipdestin();
	int getlendata();
	int getbugs() { return bugs; };
private:
	int lendata;
	iphead head;
	char lendatastr[3];
	int bugs; 
};


#endif
