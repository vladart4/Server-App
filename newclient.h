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
    enum Action {Connect, NameUpdate, Message, Private, Test};
    Q_ENUM(Action)
    QSqlDatabase db;
    quint16 blockSize;
    QString UserName;
    QString TempName;
    QSharedPointer<QString> nameptr;
    QMap<QString, NewClient*> NamesMap;
    QTcpSocket *socket;

signals:
    void error(QTcpSocket::SocketError socketerror);
    void AddName(QString, NewClient*);
    void finished(NewClient*);
    void messageToAll(QString, QString);
    void messageToOne(QString, QString, QString);

public slots:
    //void SetParent(NewServer* parserver);
    void printName(bool bAccess);
    bool SendAccess(bool bAccess);
    bool UpdateNames(QMap<QString, NewClient*> names);
    bool SendMessageToAll(QString msg, QString name);
    bool SendMessageToOne(QString msg, QString name);

private slots:
        void readyRead();
        void disconnected();
        void disconnectfromHost();

private:
    qintptr socketDescriptor;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;

};

#endif // NEWCLIENT_H
