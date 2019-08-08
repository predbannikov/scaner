#ifndef SCANER_H
#define SCANER_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QThread>

#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <unistd.h>
#include <iomanip>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>

#include <iostream>
#include "eth_frame.h"
#include "ip_frame.h"
#include "udp_frame.h"
#include "tcp_frame.h"

#include "monitor.h"

struct InputCommand {
    char ch;
    std::string str;
    bool state_input = false;
};

class Scaner : public QObject
{
    Q_OBJECT
private:
    int _kbhit();
    int sock;
public:
    explicit Scaner(QObject *parent = nullptr);
    ~Scaner();

    int engine(unsigned int opt, int *sock_, int *rc_);
signals:
    void sendMsg(QString, QByteArray, QStringList);
    void signalStop();
public slots:
};

#endif // SCANER_H
