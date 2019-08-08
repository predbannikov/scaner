#include "monitor.h"
#include <QCoreApplication>

Monitor::Monitor(QObject *parent) : QObject(parent)
{
//    while(doWork) {
//        QFileInfo fileInfo("/proc/net/tcp");
//        QEventLoop loop;
//        QTimer timer;
//        timer.setSingleShot(true);
//        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
//        timer.start(10);
//        loop.exec();
//    }

}

void Monitor::loop()
{
    QDir dir("/proc/");
    QEventLoop loop;
    QTimer timer;
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(10);
    while(doWork) {
        loop.exec();
        QFile file("/proc/net/tcp");
        if(!file.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "file not open to read";
        }
        QByteArray data = file.readAll();
        file.close();
        QList <QByteArray> arrayList = data.split('\n');
        QStringList alllistnet;
//        QHash<QString, QString> _hash;
        dir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        foreach(QString pid, dir.entryList()) {
            QString path = "/proc/" + pid + "/fd/";
            QDir dpid(path);
            dpid.setFilter(QDir::NoDotAndDotDot);
            QStringList list = dpid.entryList();

            QString name = readNameProcess(pid);
            if(name.contains(captureApp)) {                             // Ищем нашу цель - процесс в процессах
                if(!pidsTarget.contains(pid)) {
                    pidsTarget << pid;
                    qDebug() << captureApp << "PID:" << pid << name;
                }
            }

            foreach(QString str2, list) {
                QFileInfo path2 = path + str2;
                QString target = path2.symLinkTarget();                 // Получаем полную ссылку на дескриптор Inode
                if(target.contains("socket")) {
                    for(int i = 1; i < arrayList.size(); i++) {         // Перебираем список /proc/net/tcp
                        QString withOutSpace = arrayList[i].simplified();
                        QStringList listData = withOutSpace.split(' ');
                        if(listData.size() < 10 )
                            continue;
                        if(listData[9] == "0")
                            continue;
                        if(target.contains(listData[9])) {      // listData[9] - Inode , Каждый inode проверяем на соответствие с /proc/[PID]/fd/*
                            alllistnet << QString("%1|%2|%3|%4|%5").arg(listData[1]).arg(listData[2]).arg(pid).arg(listData[9]);
                        }
                    }
                }
            }

        }
        machDepend = alllistnet;                                // Совпадающие процессы с искомым Inode
    }

}

QString Monitor::readNameProcess(QString pid) {
    QFile file("/proc/" + pid + "/cmdline");
    if(!file.exists())
        return QString();
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "not open read file";
        return QString();
    }
    QString data = file.readAll();
    file.close();
    return data;
}

void Monitor::start()
{
//    QString path("su user && wine /home/user/.wine/drive_c/ProgramFiles(x86)/Alawar/Веселаяферма.Новыеприключения/game_release.exe");
//    QProcess process;
//    process.setWorkingDirectory("/home/user/.wine/drive_c/ProgramFiles(x86)/Alawar/Веселаяферма.Новыеприключения/");
////    process.setProgram(path);
//    qint64 *pid = nullptr;
//    if(!process.startDetached(path)) {
//        qDebug() << "no run program";
//    }

    qDebug() << "launch" ;
    captureApp = "wine";
    loop();
}

void Monitor::slotStop()
{
    QString pathFile = QCoreApplication::applicationDirPath() + "/dump.log";
    QDir(pathFile).remove(pathFile);
    QFile file(pathFile);
    doWork = false;
    qDebug() << file;
    if(!file.open(QFile::WriteOnly | QFile::Text | QFile::Append)) {
        qDebug() << "not open file to write";
        return;
    }
    for(int i = 0; i < dump.size(); i++)
        file.write(dump[i]);
    file.close();
}

void Monitor::test(QString port, QByteArray data, QStringList listParam)
{
    port = port.toUpper();
//    qDebug() << "test monitor" << str;
    for(int i = 0; i < machDepend.size(); i++) {
        if(machDepend[i].contains(port)) {
            for(int k = 0; k < pidsTarget.size(); k++)
                if(machDepend[i].contains(pidsTarget[k])) {
                    QString infoStr;
                    infoStr = QString("\n+---------------------------------src:%1\tdst:%2\tpsrc:%3\tdst:%4\tPID:%5\tpack%6\n|\n").arg(listParam[0]).arg(listParam[1]).arg(listParam[2]).arg(listParam[3]).arg(pidsTarget[k]).arg(listParam[4]);
                    dump.append(infoStr.toUtf8());
                    dump.append(data);
                    qDebug() << qPrintable(data);
                    return;
                }
        }
    }
}
