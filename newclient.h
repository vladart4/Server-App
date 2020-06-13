#ifndef NEWCLIENT_H
#define NEWCLIENT_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QTcpSocket>
#include <QThreadPool>
#include <QRegExp>
#include <QSqlDatabase>
#include <QMutex>
#include <QWaitCondition>


class NewServer;

class NewClient : public QObject
{
    Q_OBJECT

public:
    explicit NewClient(qintptr ID, QObject *parent = 0);
    enum Action {Connect, NameUpdate, Message};
    Q_ENUM(Action)
    QSqlDatabase db;
    quint16 blockSize;
    QString UserName;
    QSharedPointer<QString> nameptr;
    QMap<QString, NewClient*> NamesMap;
    QTcpSocket *socket;
signals:
    void error(QTcpSocket::SocketError socketerror);
    void AddName(QString, NewClient*);
    void finished(NewClient*);
    void messageToAll(QString, QString);

public slots:
    void readyRead();
    void disconnected();
    //void SetParent(NewServer* parserver);
    void printName(bool bAccess, NewClient* client);
    bool SendAccess(bool bAccess);
    bool UpdateNames(QMap<QString, NewClient*> names);
    bool SendMessageToAll(QString msg, QString name);



private:

    qintptr socketDescriptor;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;


};

#endif // NEWCLIENT_H
