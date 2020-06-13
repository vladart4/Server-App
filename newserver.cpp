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



void NewServer::debugPrintNames()
{
   //myQMap::iterator i;
   //for (i=NamesMap.begin(); i != NamesMap.end(); ++i)
       //qDebug() << i.key() << ":" << i.value() << Qt::endl ;
}

void NewServer::SlotAddName(QString name, NewClient* client)
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
    emit sendBack(name, client);
    emit grantAccess(bAccess, client);

}

void NewServer::RemoveClient(NewClient *client)
{
QString name = client->UserName;
Clients.removeAt(Clients.indexOf(client));
NamesMap.remove(name);
emit UpdateNameList(NamesMap);
}

void NewServer::SendMessageToAll(QString msg, QString name)
{
    emit SendMessageToAllSignal(msg, name);
}



// This function is called by QTcpServer when a new connection is available.
void NewServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    // Every new connection will be run in a newly created thread
    //QSharedPointer<NewClient> client = QSharedPointer<NewClient>(new NewClient(socketDescriptor), &QObject::deleteLater);
    //auto client = QSharedPointer<NewClient>::create(socketDescriptor);
    NewClient* client = new NewClient(socketDescriptor);
    Clients.append(client);
   // NewClient *client = new NewClient(socketDescriptor);
    connect(client, &NewClient::AddName, this, &NewServer::SlotAddName);
    connect(this, &NewServer::grantAccess, client, &NewClient::printName);
    connect(this, &NewServer::UpdateNameList, client, &NewClient::UpdateNames);
    connect(client, &NewClient::messageToAll, this, &NewServer::SendMessageToAll);
    connect(this, &NewServer::SendMessageToAllSignal, client, &NewClient::SendMessageToAll);
    QThread *thread = new QThread();
    client->socket->moveToThread(thread);
    client->moveToThread(thread);


    connect(client, &NewClient::finished, this, &NewServer::RemoveClient);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

