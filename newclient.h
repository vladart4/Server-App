#ifndef NEWCLIENT_H
#define NEWCLIENT_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QTcpSocket>
#include <QThreadPool>
#include <QRegExp>
#include <QMutex>
#include <iostream>
#include <QWaitCondition>
#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtperrors.h>
#include <jrtplib3/rtppacket.h>
#include <emiplib/miprtpcomponent.h>
#include <QTimer>

using namespace jrtplib;
class NewServer;

class NewClient : public QObject
{
    Q_OBJECT

public:
    explicit NewClient(qintptr ID, QObject *parent = 0);
    enum Action {Connect, NameUpdate, Message, Private, Test};
    Q_ENUM(Action)

    quint16 blockSize;
    QString UserName;
    QString TempName;
    // QSharedPointer<QString> nameptr;
    QMap<QString, NewClient*> NamesMap;
    QTcpSocket *socket;
    QTimer *timer;

signals:
    void error(QTcpSocket::SocketError socketerror);
    void addName(QString, NewClient*);
    void finished(NewClient*);
    void messageToAll(QString, QString);
    void messageToOne(QString, QString, QString);

public slots:
    //void SetParent(NewServer* parserver);
    bool sendAccess(bool bAccess, QStringList names);
    bool noticeConnect(QString name);
    bool noticeDisconnect(QString name);
    bool sendMessageToAll(QString msg, QString name);
    bool sendMessageToOne(QString msg, QString name);

private slots:
    void readyRead();
    void disconnected();
    void disconnectfromHost();
    void RTPData();

private:
    qintptr socketDescriptor;
    // QMutex mutex;
    // QWaitCondition cond;
    // bool quit;
    RTPUDPv4TransmissionParams transmissionParams;
    RTPSessionParams sessionParams;
    bool returnValue;
    int portBase = 14004;
    int status;
    RTPPacket *pack;
    uint8_t *datatest;
    RTPSession rtpSession;


};

#endif // NEWCLIENT_H
