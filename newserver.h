#ifndef NEWSERVER_H
#define NEWSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QMetaType>
#include <QTimer>
#include "newclient.h"

typedef QMap<QString, NewClient*> myQMap;
Q_DECLARE_METATYPE(myQMap);

class NewServer : public QTcpServer
{
    Q_OBJECT
    public:
        explicit NewServer(QObject *parent = 0);

        // QSqlDatabase db;
        // QTimer* timer;

        /// Список текущих клиентских подключений
        QList<NewClient*> Clients;

        /// Словарь, сопоставляющий пользователя с клиентом,
        /// нужен для быстрой выборки при отправке в приват.
        myQMap NamesMap;

        /// Поднять сервер и отчитаться в консоль
        void startServer(int port=14000);

    signals:
        void sendBack(QString, NewClient*);
        void grantAccess(bool, QStringList);
        void connectSignal(QString);
        void disconnectSignal(QString);
        void sendMessageToAllSignal(QString, QString);
        void sendMessageToOneSignal(QString, QString, NewClient*);

    public slots:
        void slotAddName(QString name, NewClient* client);
        void removeClient(NewClient* client);
        void sendMessageToAll(QString msg, QString name);
        void sendMessageToOne(QString msg, QString name, QString rcv);
        void sendCallRequest(QString reciever, QString address, QString sender);
        void makeCall(QString reciever, QString sender);
        void rejectCall(QString sender, QString reciever);

    protected:
        void incomingConnection(qintptr socketDescriptor);
};

#endif // NEWSERVER_H
