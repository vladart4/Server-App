#include "newserver.h"
#include "newclient.h"
#include "newthread.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QMetaType>
#include <QMetaObject>



NewServer::NewServer(QObject *parent) :
    QTcpServer(parent)
{

qRegisterMetaType<myQMap>();
qRegisterMetaType<qint64>();

}

void NewServer::startServer()
{
    int port = 1234;

    if(!this->listen(QHostAddress::Any, port))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << port << "...";

    }

}


void NewServer::SlotAddName(QString name, NewClient* client) //Сверяемся с существующими подключениями
{
    qDebug() << name;
    bool bAccess = true;
    if (NamesMap.contains(name))
        bAccess = false;
    else
    {
        NamesMap.insert(name, client);
        emit UpdateNameList(NamesMap);
    }
    qDebug() << bAccess;
    QMetaObject::invokeMethod(client, "printName",
                              Q_ARG(bool, bAccess));

}

void NewServer::RemoveClient(NewClient *client) //Дисконнект
{
QString name = client->UserName;
Clients.removeAt(Clients.indexOf(client));
if (name != "")
NamesMap.remove(name);
emit UpdateNameList(NamesMap);
QTimer::singleShot(50, this, std::bind(&NewServer::SendMessageToAll, this, "Has left the chat", name));
}

void NewServer::SendMessageToAll(QString msg, QString name) //Отправляем сообщения всем
{
    emit SendMessageToAllSignal(msg, name);
}

void NewServer::SendMessageToOne(QString msg, QString name, QString rcv) //Отправляем сообщение единственному получателю и отправителю
                                                                         //(у отправителя можно сделать на стороне клиента)
{
    NewClient* reciever = NamesMap[rcv];
    if (reciever)
       {
         QMetaObject::invokeMethod(reciever, "SendMessageToOne",
                                   Q_ARG(QString, msg),
                                   Q_ARG(QString, name));
       }
    NewClient* sender = NamesMap[name];
    if (sender)
    {
        QMetaObject::invokeMethod(sender, "SendMessageToOne",
                                  Q_ARG(QString, msg),
                                  Q_ARG(QString, name));
    }

}



void NewServer::incomingConnection(qintptr socketDescriptor)
{

    qDebug() << socketDescriptor << " Connecting...";
    NewClient* client = new NewClient(socketDescriptor);
    Clients.append(client);
    connect(client, &NewClient::AddName, this, &NewServer::SlotAddName);
    connect(this, &NewServer::grantAccess, client, &NewClient::printName);
    connect(this, &NewServer::UpdateNameList, client, &NewClient::UpdateNames);
    connect(client, &NewClient::messageToAll, this, &NewServer::SendMessageToAll);
    connect(this, &NewServer::SendMessageToAllSignal, client, &NewClient::SendMessageToAll);
    connect(client, &NewClient::messageToOne, this, &NewServer::SendMessageToOne);
    QThread *thread = new QThread();
    client->socket->moveToThread(thread);
    client->moveToThread(thread);


    connect(client, &NewClient::finished, this, &NewServer::RemoveClient);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

