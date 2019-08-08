#ifndef 	ETH_FRAME_H
#define 	ETH_FRAME_H

//#include <stdio.h>
 
#define 	LENETH		1518	//Длина максимального кадра
#define		LENDATA		1500	//Длина данных в ethernet frame
#define		LENMAC		6		//Длина MAC
#define		LENETHTYPE	2		//Длина заголовка типа ethernet пакета
#define 	LENCRC		4		//Длина заголовка crc checksum
#define		ETHERTYPE	0x0800	//Код IP протокола



struct eth{
	int d[LENMAC];
	int s[LENMAC];
	unsigned char strd[17];	// Строка с адрессом в формате FF:FF:FF:FF:FF:FF
	unsigned char strs[17];
	int type[LENETHTYPE];
	char strt[4];
	unsigned long crc[LENCRC];
};

class eth_frame{
public:
	int incapeth(unsigned char *buffer, int lenrecvpack);	//Тут получает наш класс данные
	int* getmacd();		//Получить МАС назначения
	int* getmacs();		//Получить МАС исходящего устройства
	int getethtype();	//Получить тип фрейма
	unsigned long getcrc();
    int data[1500];
    int getlendata() {return lendata; }
private:
	eth str;
	int lendata;		//Длина данных которые несёт кадр
};


#endif
