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
#include <QWaitCondition>


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

signals:
    void error(QTcpSocket::SocketError socketerror);
    void addName(QString, NewClient*);
    void finished(NewClient*);
    void messageToAll(QString, QString);
    void messageToOne(QString, QString, QString);

public slots:
    //void SetParent(NewServer* parserver);
    bool sendAccess(bool bAccess);
    bool noticeConnect(QString name);
    bool noticeDisconnect(QString name);
    bool sendMessageToAll(QString msg, QString name);
    bool sendMessageToOne(QString msg, QString name);

private slots:
    void readyRead();
    void disconnected();
    void disconnectfromHost();

private:
    qintptr socketDescriptor;
    // QMutex mutex;
    // QWaitCondition cond;
    // bool quit;
};

#endif // NEWCLIENT_H
