#include "scaner.h"

using std::cout;
using std::endl;

Scaner::Scaner(QObject *parent) : QObject(parent)
{
    int rc;  // с ним связываем наш интерфейс

    int optval;
    int optlen;

    QThread *thread = new QThread(this);
    Monitor *monitor = new Monitor;
    QObject::connect(thread, &QThread::finished, monitor, &QObject::deleteLater);
    QObject::connect(thread, &QThread::started, monitor, &Monitor::start);
    QObject::connect(this, &Scaner::sendMsg, monitor, &Monitor::test);
    QObject::connect(this, &Scaner::signalStop, monitor, &Monitor::slotStop);

    monitor->moveToThread(thread);
    thread->start();

    optlen = sizeof(optval);
    cout << "rc = " << optlen << endl;

    sock = socket(AF_PACKET,SOCK_PACKET,htons(ETH_P_ALL));
    rc = setsockopt(sock,SOL_SOCKET, SO_BINDTODEVICE, "enp30s0\x00", strlen("enp30s0\x00")+1); // связывание интерфейса
    if (rc) {
        cout << "false"  << strerror(errno) << endl ;
        return;
    }
    cout << "rc true" << endl;
    engine(0, &sock, &rc);
//    close(sock);
}

Scaner::~Scaner()
{
    close(sock);
}

int Scaner::_kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, nullptr);
        initialized = true;
    }
    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

int Scaner::engine(unsigned int opt, int *sock_, int *rc_)
{
    unsigned char buf[LENETH];
    eth_frame frame;
    ip_frame ipfr;
    tcp tcpfr;
    udp udpfr;
    int i, bt;
    int *tmp;
//    opt = 128;				// Для отладки
    unsigned int port = 0;
    InputCommand incom;
//    port = opt >> 8;
    port = 80;
    for(int count = 0; opt < 256 ;count++) {

        if(_kbhit()) {
            int ch = getchar(); //static_cast<char>(getchar());
            if(incom.state_input) {
                if(ch == '\n') {
                    port = static_cast<unsigned int>(std::stoi(incom.str));
                    incom.str.clear();
                    incom.state_input = false;
                }
                incom.str.append(1, static_cast<char>(ch));
            } else {
                if(ch == '1') {
                    if((opt & 128) >> 7)
                        opt &= static_cast<unsigned int>(~128);
                    else
                        opt |= 128;
                } else if(ch == '2') {
                    if((opt & 64) >> 6)
                        opt &= static_cast<unsigned int>(~64);
                    else
                        opt |= 64;
                } else if(ch == '3') {
                    if((opt & 32) >> 5)
                        opt &= static_cast<unsigned int>(~32);
                    else
                        opt |= 32;
                } else if(ch == '4') {                                  // Отобразить весь данные ETHERNET пакета
                    if((opt & 16) >> 4)
                        opt &= static_cast<unsigned int>(~16);
                    else
                        opt |= 16;
                } else if(ch == '5') {                                  // Отобразить данные IP пакета
                    if((opt & 8) >> 3)
                        opt &= static_cast<unsigned int>(~8);
                    else
                        opt |= 8;
                } else if(ch == '6') {                                  // Отобразить данные TCP
                    if((opt & 4) >> 2)
                        opt &= static_cast<unsigned int>(~4);
                    else
                        opt |= 4;
                } else if(ch == '7') {
                    if((opt & 2) >> 1)
                        opt &= static_cast<unsigned int>(~2);
                    else
                        opt |= 2;
                } else if(ch == '7') {
                    if((opt & 2) >> 1)
                        opt &= static_cast<unsigned int>(~2);
                    else
                        opt |= 2;
                } else if(ch == 'q') {
                    opt |= 256;
                } else if(ch == 'p') {
                    incom.state_input = true;
                }
            }

        }
        *rc_ = recvfrom(*sock_,buf,sizeof(buf),0,0,0);

        frame.incapeth(buf,*rc_);
//        cout << endl <<  "#####################################################";
                    // ETHERNET заголовок
        if (128 & opt){
            cout << endl << "+---------------------------------------------------+";
            cout << endl << "|                ETHERNET FRAME                     |";
            cout << endl << "+----------------------+----------------------------+";
            cout << endl << "| MAC destination      | ";
            tmp = frame.getmacd();
            cout.setf(std::ios::uppercase);
            for (i = 0; i < 6; i++){
                cout << std::setw(2) << std::setfill('0') << std::hex << tmp[i];
                if (i < 5) cout << ":";
            };
            cout << "          |";
            cout << endl << "| MAC source           | ";
            tmp = frame.getmacs();
            cout.setf(std::ios::uppercase);
            for (i = 0; i < 6; i++){
                cout << std::setw(2) << std::setfill('0') << std::hex << tmp[i];
                if (i < 5) cout << ":";
            };
            cout << "          |";
            cout << endl << "| CRC                  | ";
            cout << std::setw(17) << std::setfill(' ') << std::dec << frame.getcrc();
            cout << "          |";
            cout << endl << "| Type incapsulation   | ";
            cout << std::setw(17) << std::setfill(' ')<< std::hex << frame.getethtype() ;
            cout << "          |";
            cout << endl << "+----------------------+----------------------------+";
            cout << std::dec;
            cout.unsetf(std::ios::uppercase);
        }
                    // Данные ETHERNET
        if (16 & opt){
            bt = 1;
            cout << endl << "+----------------------+----------------------------+";
            cout << endl << "| ";
            for (i=0; i < frame.getlendata(); i++){
                cout.setf(std::ios::uppercase);
                cout << std::setw(2) << std::setfill('0') << std::hex << frame.data[i];
                if (bt == 8) cout << "   ";
                else if (bt == 16) {
                    bt = 0;
                    cout << " |";
                    if (i != (frame.getlendata()-1)) cout << endl << "| ";
                } else cout << " ";
                bt++;
                if ((i == (frame.getlendata()-1)) && (bt != 1)) {
                    int j = (frame.getlendata()%16);
                    if (j <= 8) j=j*3;
                    else j=j*3 +2;
                    cout << std::setw(51-j) << std::setfill(' ') << "|";
                };
            };
            cout << endl << "+---------------------------------------------------+";
            cout << std::dec;
            cout.unsetf(std::ios::uppercase);
        }
                    // Если тип передаваемому протоколу IP
        if (frame.getethtype() == ETHERTYPE) {
            ipfr.incapip(frame.data,frame.getlendata());
                    // отобразить заголовок IP
            if (64 & opt){
                cout << endl << "+---------------------------------------------------+";
                cout << endl << "|               INTERNET PROTOCOL                   |";
                cout << endl << "+----------------------+----------------------------+";
                cout << endl << "| Version IP           | ";
                cout << std::setw(17) << std::setfill(' ') << ipfr.getversion();
                cout << "          |";
                cout << endl << "| length head          | ";
                cout << std::setw(17) << std::setfill(' ') << ipfr.getsizehead();
                cout << "          |";
                cout << endl << "| servis in hex        | ";
                cout << std::setw(17) << std::setfill(' ') << std::hex << ipfr.getservis();
                cout << "          |";
                cout << endl << "| length packet        | ";
                cout << std::setw(17) << std::setfill(' ') << std::dec << ipfr.getsizepack();
                cout << "          |";
                cout << endl << "| identificator        | ";
                cout << std::setw(17) << std::setfill(' ') << std::hex << ipfr.getidentif();
                cout << "          |";
                cout << endl << "| flags                | ";
                cout << std::setw(17) << std::setfill(' ') << std::hex << ipfr.getflags();
                cout << "          |";
                cout << endl << "| fragmen identif      | ";
                cout << std::setw(17) << std::setfill(' ') << std::hex << ipfr.getsfrag();
                cout << "          |";
                cout << endl << "| time live TTL        | ";
                cout << std::setw(17) << std::setfill(' ') << std::dec << ipfr.gettimeTTL();
                cout << "          |";
                cout << endl << "| protocol up          | ";
                cout << std::setw(17) << std::setfill(' ') << std::dec << ipfr.getprot();
                cout << "          |";
                cout << endl << "| controlsum head      | ";
                cout << std::setw(17) << std::setfill(' ') << ipfr.getcontrolsum();
                cout << "          |";
                cout << endl << "| ip source            | ";
                cout << std::setw(17) << std::setfill(' ') << ipfr.getipsource();
                cout << "          |";
                cout << endl << "| ip destination       | ";
                cout << std::setw(17) << std::setfill(' ') << ipfr.getipdestin();
                cout << "          |";
                cout << endl << "+----------------------+----------------------------+";
                cout << std::dec;
                cout.unsetf(std::ios::uppercase);
            }
                    // Отобразить данные несущие пакеты IP
            if (8 & opt) {
                bt = 1;
                cout << endl << "+----------------------+----------------------------+";
                cout << endl << "| ";
                for (i=0; i < ipfr.getlendata(); i++){
                    cout.setf(std::ios::uppercase);
                    cout << std::setw(2) << std::setfill('0') << std::hex << ipfr.data[i];
                    if (bt == 8) cout << "   ";
                    else if (bt == 16) {
                        bt = 0;
                        cout << " |";
                        if (i != (ipfr.getlendata()-1)) cout << endl << "| ";
                    } else cout << " ";
                    bt++;
                    if ((i == (ipfr.getlendata()-1)) && (bt != 1)) {
                        int j = (ipfr.getlendata()%16);
                        if (j <= 8) j=j*3 + 2;
                        else j=j*3 +2;
                        cout << std::setw(51-j) << std::setfill(' ') << "|";
                    };
                };
                cout << endl << "+---------------------------------------------------+";
                cout << std::dec;
                cout.unsetf(std::ios::uppercase);
            }

            tcpfr.tcpincap(ipfr.data, ipfr.getlendata());
            QByteArray array;
            for(int d = 0; d < tcpfr.getlendata(); d++) {
                array.append(static_cast<char>(tcpfr.data[d]));
            }
//            qDebug() << ipSource;
            QStringList listParam;
            listParam << QString::fromLocal8Bit(ipfr.getipsource());
            listParam << QString::fromLocal8Bit(ipfr.getipdestin());
            listParam << QString::number(tcpfr.getportd());
            listParam << QString::number(tcpfr.getports());

            listParam << QString::number(count);
            emit sendMsg(QString::number(tcpfr.getportd(), 16), array, listParam);
//            if(ipSource == "127.0.0.1" || ipSource == "192.168.0.102")
//            else
//                emit sendMsg(QString::number(tcpfr.getportd(), 16), array, 1, count);

                    // Протокол транспортного уровня
            if ((32 & opt) || (4 & opt) || (2 & opt)){
                    // Если TCP
                if (ipfr.getprot() == 6) {
//                    readProcNet(static_cast<int>(tcpfr.getportd()));
                    // Если отобразить заголовок
                    if (32 & opt) {
                        cout << endl << "+---------------------------------------------------+";
                        cout << endl << "|           TRANSMISSION CONTROL PROTOCOL           |";
                        cout << endl << "+----------------------+----------------------------+";
                        cout << endl << "| Port source          | ";
                        cout << std::dec << std::setw(17) << std::setfill(' ') << tcpfr.getports() << "  "<< std::hex << tcpfr.getports();
                        cout << "    |" ;
                        cout << endl << "| Port destination     | ";
                        cout << std::dec << std::setw(17) << std::setfill(' ') << tcpfr.getportd() << "  "<< std::hex << tcpfr.getportd();
                        cout << "    |";
                        cout << endl << "| Length packet        | ";
                        cout << std::dec << std::setw(17) << std::setfill(' ') << tcpfr.getlendata();
                        cout << "          |";
                        cout << endl << "+---------------------------------------------------+";
                    };
                    // Если вывести данные
                    if (4 & opt) {
                        bt = 1;
                        cout << endl << "+----------------------+----------------------------+";
                        cout << endl << "| ";
                        for (i=0; i < tcpfr.getlendata(); i++){
                            cout.setf(std::ios::uppercase);
                            cout << std::setw(2) << std::setfill('0') << std::hex << tcpfr.data[i];
                            if (bt == 8) cout << "   ";
                            else if (bt == 16) {
                                bt = 0;
                                cout << " |";
                                if (i != (tcpfr.getlendata()-1)) cout << endl << "| ";
                            } else cout << " ";
                            bt++;
                            if ((i == (tcpfr.getlendata()-1)) && (bt != 1)) {
                                int j = (tcpfr.getlendata()%16);
                                if (j <= 8) j=j*3;
                                else j=j*3 +2;
                                cout << std::setw(51-j) << std::setfill(' ') << "|";
                            };
                        };
                        cout << endl << "+---------------------------------------------------+";
                        cout << std::dec;
                        cout.unsetf(std::ios::uppercase);


                    };
                    //  если UDP
                } else if (ipfr.getprot() == 17){
                    udpfr.udpincap(ipfr.data, ipfr.getlendata());
                    // Если отобразить заголовок
                    if (32 & opt){
//                        cout << std::dec;
//                        cout << endl << "+---------------------------------------------------+";
//                        cout << endl << "|              USER DATAGRAM PROTOCOL               |";
//                        cout << endl << "+----------------------+----------------------------+";
//                        cout << endl << "| Port source          | ";
//                        cout << std::setw(17) << std::setfill(' ') << udpfr.getports();
//                        cout << "          |";
//                        cout << endl << "| Port destination     | ";
//                        cout << std::setw(17) << std::setfill(' ') << udpfr.getportd();
//                        cout << "          |";
//                        cout << endl << "| Length packet        | ";
//                        cout << std::setw(17) << std::setfill(' ') << udpfr.getlendata();
//                        cout << "          |";
//                        cout << endl << "+---------------------------------------------------+";
                    }
                    // Если отобразить данные
                    if (4 & opt) {
                        bt = 1;
                        cout << endl << "+---------------------------------------------------+";
                        cout << endl << "| ";
                        for (i=0; i < udpfr.getlendata(); i++){
                            cout.setf(std::ios::uppercase);
                            cout << std::setw(2) << std::setfill('0') << std::hex << udpfr.data[i];
                            if (bt == 8) cout << "   ";
                            else if (bt == 16) {
                                bt = 0;
                                cout << " |";
                                if (i != (udpfr.getlendata()-1)) cout << endl << "| ";
                            } else cout << " ";
                            bt++;
                            if ((i == (udpfr.getlendata()-1)) && (bt != 1)) {
                                int j = (udpfr.getlendata()%16);
                                if (j <= 8) j=j*3 + 2;
                                else j=j*3 +2;
                                cout << std::setw(51-j) << std::setfill(' ') << "|";
                            };
                        };
                        cout << endl << "+---------------------------------------------------+";
                        cout << std::dec;
                        cout.unsetf(std::ios::uppercase);
                    }
                    if (2 & opt){
                        cout << "tcp frame port =" << port << "framePort =" << tcpfr.getports();
                        if(ipfr.getprot() == 6) {
                            cout << endl << "+---------------------------------------------------+";
                            cout << endl << "|                  DATA PORT HEX                    |";
                            cout << endl << "+---------------------------------------------------+";
                            if (port == tcpfr.getports()){
                                cout << endl << "| incoming data                                     |";
                                cout << endl << "+---------------------------------------------------+";
                                cout << endl << "| ";
                                bt = 1;
                                for (i=0; i < tcpfr.getlendata(); i++){
                                    cout.setf(std::ios::uppercase);
                                    cout << std::setw(2) << std::setfill('0') << std::hex << tcpfr.data[i];
                                    if (bt == 8) cout << "   ";
                                    else if (bt == 16) {
                                        bt = 0;
                                        cout << " |";
                                        if (i != (tcpfr.getlendata()-1)) cout << endl << "| ";
                                    } else cout << " ";
                                    bt++;
                                    if ((i == (tcpfr.getlendata()-1)) && (bt != 1)) {
                                        int j = (tcpfr.getlendata()%16);
                                        if (j < 8) j=j*3;
                                        else j=j*3 +2;
                                        cout << std::setw(51-j) << std::setfill(' ') << "|";
                                    };
                                };
                                cout << std::dec;
                                cout.unsetf(std::ios::uppercase);
                                cout << endl << "+---------------------------------------------------+";
                            }
                            if (port == tcpfr.getportd()){
                                cout << endl << "| Sent data                                         |";
                                cout << endl << "+---------------------------------------------------+";
                                cout << endl << "| ";
                                bt = 1;
                                for (i=0; i < tcpfr.getlendata(); i++){
                                    cout.setf(std::ios::uppercase);
                                    cout << std::setw(2) << std::setfill('0') << std::hex << tcpfr.data[i];
                                    if (bt == 8) cout << "   ";
                                    else if (bt == 16) {
                                        bt = 0;
                                        cout << " |";
                                        if (i != (tcpfr.getlendata()-1)) cout << endl << "| ";
                                    } else cout << " ";
                                    bt++;
                                    if ((i == (tcpfr.getlendata()-1)) && (bt != 1)) {
                                        int j = (tcpfr.getlendata()%16);
                                        if (j < 8) j=j*3;
                                        else j=j*3 +2;
                                        cout << std::setw(51-j) << std::setfill(' ') << "|";
                                    };
                                };
                                cout << std::dec;
                                cout.unsetf(std::ios::uppercase);
                                cout << endl << "+---------------------------------------------------+";
                            }
                        }
                        if(ipfr.getprot() == 17) {
                            cout << endl << "+---------------------------------------------------+";
                            cout << endl << "|                  DATA PORT HEX                    |";
                            cout << endl << "+---------------------------------------------------+";
                            if (port == udpfr.getports()){
                                cout << endl << "| incoming data                                     |";
                                cout << endl << "+---------------------------------------------------+";
                                cout << endl << "| ";
                                bt = 1;
                                for (i=0; i < udpfr.getlendata(); i++){
                                    cout.setf(std::ios::uppercase);
                                    cout << std::setw(2) << std::setfill('0') << std::hex << udpfr.data[i];
                                    if (bt == 8) cout << "   ";
                                    else if (bt == 16) {
                                        bt = 0;
                                        cout << " |";
                                        if (i != (udpfr.getlendata()-1)) cout << endl << "| ";
                                    } else cout << " ";
                                    bt++;
                                    if ((i == (udpfr.getlendata()-1)) && (bt != 1)) {
                                        int j = (udpfr.getlendata()%16);
                                        if (j < 8) j=j*3;
                                        else j=j*3 +2;
                                        cout << std::setw(51-j) << std::setfill(' ') << "|";
                                    };
                                };
                                cout << std::dec;
                                cout.unsetf(std::ios::uppercase);
                                cout << endl << "+---------------------------------------------------+";
                            };
                            if (port == udpfr.getportd()){
                                cout << endl << "| Sent data                                         |";
                                cout << endl << "+---------------------------------------------------+";
                                cout << endl << "| ";
                                bt = 1;
                                for (i=0; i < udpfr.getlendata(); i++){
                                    cout.setf(std::ios::uppercase);
                                    cout << std::setw(2) << std::setfill('0') << std::hex << udpfr.data[i];
                                    if (bt == 8) cout << "   ";
                                    else if (bt == 16) {
                                        bt = 0;
                                        cout << " |";
                                        if (i != (udpfr.getlendata()-1)) cout << endl << "| ";
                                    } else cout << " ";
                                    bt++;
                                    if ((i == (udpfr.getlendata()-1)) && (bt != 1)) {
                                        int j = (udpfr.getlendata()%16);
                                        if (j < 8) j=j*3;
                                        else j=j*3 +2;
                                        cout << std::setw(51-j) << std::setfill(' ') << "|";
                                    };
                                };
                                cout << std::dec;
                                cout.unsetf(std::ios::uppercase);
                                cout << endl << "+---------------------------------------------------+";
                            };
                        }


                    };
                };
            };
        };
        cout << std::flush;
        cout << incom.str;
        // Сделать выход из цикла по нажатию клавишь
    }
    emit signalStop();
    return 0;
};
