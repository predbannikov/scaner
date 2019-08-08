#ifndef MONITOR_H
#define MONITOR_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QProcess>
#include <QEventLoop>
#include <QQueue>
#include <QTimer>


class Monitor : public QObject
{
    Q_OBJECT
public:
    explicit Monitor(QObject *parent = nullptr);
private:
    void loop();
    bool doWork = true;
//    QTimer timer;
    QHash<QString, QString> hash;
    QString readNameProcess(QString pid);
    QStringList machDepend;
    QString captureApp;
    QStringList pidsTarget;
    QList<QByteArray> dump;
    QQueue<QStringList> queueMachDepends;
signals:

public slots:
    void start();
    void slotStop();
    void test(QString port, QByteArray data, QStringList listParam);

};

#endif // MONITOR_H
