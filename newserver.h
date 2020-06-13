#ifndef NEWSERVER_H
#define NEWSERVER_H

#include <QObject>
#include <QTcpServer>
#include "newthread.h"
#include "newclient.h"
#include "QSqlDatabase"
#include <QMetaType>
#include <QTimer>

typedef QMap<QString, NewClient*> myQMap;
Q_DECLARE_METATYPE(myQMap);

class NewServer : public QTcpServer
{
    Q_OBJECT
    public:
        explicit NewServer(QObject *parent = 0);
        QList<NewThread*> ConnectedUsrs;
        void startServer();
        QSqlDatabase db;
        myQMap NamesMap;
        QList<QTcpSocket*> Sockets;
        QList<NewClient*> Clients;
        QTimer* timer;

    signals:
        void sendBack(QString, NewClient*);
        void grantAccess(bool, NewClient*);
        void UpdateNameList(myQMap);
        void SendMessageToAllSignal(QString, QString);
        void SendMessageToOneSignal(QString, QString, NewClient*);


    public slots:
        void SlotAddName(QString name, NewClient* client);
        void RemoveClient(NewClient* client);
        void SendMessageToAll(QString msg, QString name);
        void SendMessageToOne(QString msg, QString name, QString rcv);

    protected:
        void incomingConnection(qintptr socketDescriptor);
};

#endif // NEWSERVER_H
