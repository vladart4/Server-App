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
#include <QTimer>

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
    QHostAddress *ipv4address;
    bool goodDisconnect = false;

    bool isCalling();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void addName(QString, NewClient*);
    void refreshUsersSignal(QString);
    void finished(NewClient*);
    void messageToAll(QString, QString);
    void messageToOne(QString, QString, QString);
    void requestCallSignal(QString, QString, QString);
    void makeCallConnect(QString, QString);
    void makeCallReject(QString, QString);

public slots:
    //void SetParent(NewServer* parserver);
    bool sendAccess(bool bAccess, QStringList names);
    bool refreshUsers(QStringList names);
    bool noticeConnect(QString name);
    bool noticeDisconnect(QString name, bool atWill);
    bool sendMessageToAll(QString msg, QString name);
    bool sendMessageToOne(QString msg, QString name);
    bool sendCallRequest(QString name, QString address);
    bool sendMakeCall(QString recieve, QString address);
    bool sendRejectCall(QString name);

private slots:
    void readyRead();
    void disconnected();
    void disconnectfromHost();


private:
    qintptr socketDescriptor;
    // QMutex mutex;
    // QWaitCondition cond;
    // bool quit;
    QString connectedToName;
    QString connectedToAddress;

};

#endif // NEWCLIENT_H
