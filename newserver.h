#ifndef NEWSERVER_H
#define NEWSERVER_H

#include <QObject>
#include <QTcpServer>
#include "newthread.h"
#include "newclient.h"
#include "QSqlDatabase"
#include <QMetaType>

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
        void debugPrintNames();
        myQMap NamesMap;
        QList<QTcpSocket*> Sockets;
        QList<NewClient*> Clients;

    signals:
        void sendBack(QString, NewClient*);
        void grantAccess(bool, NewClient*);
        void UpdateNameList(myQMap);
        void SendMessageToAllSignal(QString, QString);


    public slots:
        void SlotAddName(QString name, NewClient* client);
        void RemoveClient(NewClient* client);
        void SendMessageToAll(QString msg, QString name);

    protected:
        void incomingConnection(qintptr socketDescriptor);
};

#endif // NEWSERVER_H
