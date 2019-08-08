#ifndef		TRANSPORT_H
#define		TRANSPORT_H
struct udplongd{
	unsigned int ports;
	unsigned int portd;
	int lendata;
	unsigned int checksum;
};
struct udpdigit{
	unsigned int ports[2];
	unsigned int portd[2];
	int lendata[2];
	unsigned int checksum[2]; 
};
struct udppack{
	udpdigit d;
	udplongd l;
};
class udp{
public:
	int data[1500];
	int udpincap(int *buffer, int lenframe);
    int getlendata() {return lendata;}
	unsigned int getports();
	unsigned int getportd();
	unsigned int getchecksum();
private:
	udppack pack;
	int lendata;
};


#endif
