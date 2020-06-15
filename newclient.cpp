#include "newclient.h"
#include "qdatastream.h"
#include "QSqlDatabase"
#include "QSqlQuery"
#include "newserver.h"
#include <QTimer>


NewClient::NewClient(qintptr ID, QObject *parent) : QObject(parent)
{
    socketDescriptor = ID;
    blockSize = 0;
    socket = new QTcpSocket();

    if(!socket->setSocketDescriptor(socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    qDebug() << socketDescriptor << " Client connected";
}


void NewClient::readyRead()
{
    // get the information
    //QByteArray Data = socket->readAll();

    // will write on server side window
    //qDebug() << socketDescriptor << " Data in: " << Data;

    //socket->write(Data);

    QDataStream in(socket);

    if (blockSize == 0) {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blockSize;
    }

    if (socket->bytesAvailable() < blockSize)
        return;
    else
        blockSize = 0;

    Action command;
    in >> command;
    qDebug() << socketDescriptor << "Received command " << command;

    switch(command)
    {
        case Connect:
        {
            QString name;
            in >> name;

            qDebug() << socketDescriptor << "Recieve name" << name;

            TempName = name;
            emit addName(TempName, this);
            //SendAccess(true);
            break;
        }
        case Message:
        {
            QString name;
            QString msg;

            in >> name;
            in >> msg;

            qDebug() << socketDescriptor << name << msg;

            emit messageToAll(msg, name);

            break;
        }
        case Private:
        {
            QString name;
            QString msg;
            QString rcv;

            in >> name;
            in >> msg;
            in >> rcv;

            qDebug() << socketDescriptor << name << " to " << rcv << msg;

            emit messageToOne(msg,name,rcv);
            break;
        }
        default:
        {
            disconnectfromHost();
            break;
        }
    }
}


void NewClient::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    emit finished(this);
    deleteLater();
}


// Даем доступ или отказываем
bool NewClient::sendAccess(bool bAccess)
{
    if(socket->state() != QAbstractSocket::ConnectedState)
        return false;

    if (bAccess)
        qDebug() << socketDescriptor << "Welcome to chat " + TempName;
    else
        qDebug() << socketDescriptor << "Username " + TempName + " is already taken.";


    QByteArray barr;
    QDataStream stream (&barr, QIODevice::WriteOnly);
    Action ac = Action::Connect;

    stream << ac;
    stream << bAccess;
    if(bAccess)
    {
        QList<QString> newnames;
        QMap<QString, NewClient*>::iterator i;
        for (i=NamesMap.begin(); i != NamesMap.end(); ++i)
        newnames << i.key();
        stream << newnames;
        UserName = TempName;
    }
    else
    {
        QTimer::singleShot(100, this, &NewClient::disconnectfromHost);
    }

    socket->write(barr);
    //socket->flush();
    return socket->waitForBytesWritten();
}


//TODO Отсылать целый список только новым подключениям, старым отсылать имя из нового
bool NewClient::updateNames(QMap<QString, NewClient*> names)
{
    if(socket->state() != QAbstractSocket::ConnectedState)
        return false;

    NamesMap = names;
    Action ac = NameUpdate;
    QByteArray arr;
    QDataStream stream (&arr, QIODevice::WriteOnly);

    QList<QString> newnames;
    QMap<QString, NewClient*>::iterator i;
    for (i=NamesMap.begin(); i != NamesMap.end(); ++i)
        newnames << i.key();
    stream << ac;
    stream << newnames;

    socket->write(arr);
    //socket->flush();
    return socket->waitForBytesWritten();
}


// Сообщения для всех
bool NewClient::sendMessageToAll(QString msg, QString name)
{
    if(socket->state() != QAbstractSocket::ConnectedState)
        return false;

    Action ac = Message;
    QByteArray marr;
    QDataStream stream (&marr, QIODevice::WriteOnly);

    stream << ac;
    stream << msg;
    stream << name;

    socket->write(marr);
    //socket->flush();
    return socket->waitForBytesWritten();
}


bool NewClient::sendMessageToOne(QString msg, QString name) //Приватные сообщения (можно объединить это с верхним через флаги, разница у них в посылаемой команде)
{
    // qDebug() << socketDescriptor << "PrivateSlotTest" << name << msg;
    if(socket->state() != QAbstractSocket::ConnectedState)
        return false;

    Action ac = Private;
    QByteArray parr;
    QDataStream stream (&parr, QIODevice::WriteOnly);

    stream << ac;
    stream << msg;
    stream << name;

    socket->write(parr);
    //socket->flush();
    return socket->waitForBytesWritten();
}


void NewClient::disconnectfromHost()
{
    socket->disconnectFromHost();
}
