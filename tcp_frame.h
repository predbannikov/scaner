#ifndef		TCP_FRAME_H
#define		TCP_FRAME_H
struct tcpdigit{
	int ports[2];
	int portd[2];
	int nposled[4];
	int npodtver[4];
	int scdata;
	int reserv;
	int flags;
	int sizewindow[2];
	int checksum[2];
	int point[2];
	int option[4];
};
struct tcplongd{
	unsigned int ports;
	unsigned int portd;
	unsigned int checksum;
};
struct tcppack{
	tcpdigit d;
	tcplongd l;
};

class tcp{
public:
	int data[1500];
	int tcpincap(int *buffer, int lenframe);
	unsigned int getports();
	unsigned int getportd();
	int getlendata();
	tcppack pack;
private:
	int lendata;
};


#endif
