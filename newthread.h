#ifndef NEWTHREAD_H
#define NEWTHREAD_H

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

class NewThread : public QThread
{
    Q_OBJECT

public:
    explicit NewThread(qintptr ID, QObject *parent = 0);
    enum Action {Verify, Login, Access, NameUpdate};
    Q_ENUM(Action)
    void run();
    QSqlDatabase db;
    quint16 blockSize;
    QString UserName;
    QSharedPointer<QString> nameptr;
    QMap<QString, qint64> NamesMap;
signals:
    void error(QTcpSocket::SocketError socketerror);
    void AddName(QString, qint64);


public slots:
    void readyRead();
    void disconnected();
    //void SetParent(NewServer* parserver);
    void printName(bool bAccess, qint64 ID);
    void SendAccess(bool bAccess);
    void UpdateNames(QMap<QString, qint64> names);


private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;


};

#endif // NEWTHREAD_H
