#include "newserver.h"
#include "newclient.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QMetaType>
#include <QMetaObject>


NewServer::NewServer(QObject *parent) : QTcpServer(parent)
{
    qRegisterMetaType<myQMap>();
    qRegisterMetaType<qint64>();
    qRegisterMetaType<QStringList>();
}


void NewServer::startServer(int port)
{
    if(!this->listen(QHostAddress::Any, port))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << port << "...";
    }
}


// Обработка входа в чат (пользователь представился)
void NewServer::slotAddName(QString name, NewClient* client)
{
    // Проверяем, что клиент не указал существующее имя
    bool bAccess = !NamesMap.contains(name);
    QStringList names;
    if (bAccess) // Если нет, добавляем пользователя в список
    {
        NamesMap.insert(name, client);
        names = NamesMap.keys();
        emit connectSignal(name);
    }

    qDebug() << name << bAccess;

    // Даём клиенту знать насчёт возможности входа
    QTimer::singleShot(50, client,
        std::bind(&NewClient::sendAccess, client, bAccess, names));
}


// Обработка дисконнекта
void NewServer::removeClient(NewClient *client)
{
    QString name = client->UserName;
    // KILLME Clients.removeAt(Clients.indexOf(client));
    if (name != "")
    {
        NamesMap.remove(name);
        QTimer::singleShot(50, this,
            std::bind(&NewServer::disconnectSignal, this, name));
   }
}


// Отправляем сообщения всем
void NewServer::sendMessageToAll(QString msg, QString name)
{
    emit sendMessageToAllSignal(msg, name);
}


// Отправляем сообщение единственному получателю и отправителю
// (у отправителя можно сделать на стороне клиента)
// TODO: НУЖНО СДЕЛАТЬ НА СТОРОНЕ КЛИЕНТА
void NewServer::sendMessageToOne(QString msg, QString name, QString rcv)
{
    if (NamesMap.contains(rcv))
    {
        NewClient* reciever = NamesMap[rcv];
        QMetaObject::invokeMethod(reciever,
            std::bind(&NewClient::sendMessageToOne, reciever, msg, name));
    }
    if (NamesMap.contains(name))
    {
        NewClient* sender = NamesMap[name];
        QMetaObject::invokeMethod(sender,
            std::bind(&NewClient::sendMessageToOne, sender, msg, name));
    }
}


// Обработка нового подключения
// (добавить в клиенты, ждать, пока пришлёт имя)
void NewServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << socketDescriptor << " Connecting...";
    NewClient* client = new NewClient(socketDescriptor);
    // KILLME Clients.append(client);

    // Подключаем сигналы
    connect(this, &NewServer::grantAccess, client, &NewClient::sendAccess);
    connect(this, &NewServer::sendMessageToAllSignal, client, &NewClient::sendMessageToAll);
    connect(this, &NewServer::connectSignal, client, &NewClient::noticeConnect);
    connect(this, &NewServer::disconnectSignal, client, &NewClient::noticeDisconnect);
    connect(client, &NewClient::addName, this, &NewServer::slotAddName);
    connect(client, &NewClient::messageToAll, this, &NewServer::sendMessageToAll);
    connect(client, &NewClient::messageToOne, this, &NewServer::sendMessageToOne);

    // Выводим клиента в отдельный поток, чтобы удобнее
    // фиксировать его отключение
    QThread *thread = new QThread();
    client->socket->moveToThread(thread);
    client->moveToThread(thread);
    client->timer->moveToThread(thread);

    connect(client, &NewClient::finished, this, &NewServer::removeClient);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

