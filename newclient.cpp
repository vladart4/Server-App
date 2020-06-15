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


QString cutStringWithLength(QString& src)
{
    int index = src.indexOf("_");
    if (index == -1) return "";

    int msgSize = src.left(index).toInt();
    if (msgSize == 0) return "";

    src.remove(0, index+1);
    if (msgSize > src.size()) return "";

    QString answer = src.left(msgSize);
    src.remove(0, msgSize+1);

    return answer;
}

QByteArray prepareMessage(QString msg) {
    QByteArray l;
    QDataStream stream (&l, QIODevice::WriteOnly);
    stream << quint16(0);
    stream << msg;
    stream.device()->seek(0);
    stream << quint16(l.size() - sizeof(quint16));
    return l;
}

void NewClient::readyRead()
{
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

    QString command;
    in >> command;

    qDebug() << socketDescriptor << "Received " << command;

    if (!command.startsWith("EVMp"))
    {
        qDebug() << socketDescriptor << "Bad header!";
        disconnectfromHost();
    }
    command.remove(0,5); //удаляем EVMp_

    QString cmdType = command.left(command.indexOf("_"));
    command.remove(0, cmdType.length()+1);

    if (cmdType == "CONNECT")
    {
        QString name = cutStringWithLength(command);
        if (name == "")
        {
            qDebug() << socketDescriptor << "Bad parameter!";
            disconnectfromHost();
        }
        qDebug() << socketDescriptor << "Got name" << name;

        TempName = name;
        emit addName(TempName, this);
    }
    else if (UserName == "")
    {
        qDebug() << socketDescriptor << "Command without auth!";
        disconnectfromHost();
    }
    else if (cmdType == "SENDMSG")
    {
        QString msg = command;
        qDebug() << socketDescriptor << msg;

        emit messageToAll(msg, UserName);
    }
    else if (cmdType == "PRIVATEMSG")
    {
        QString rcvName = cutStringWithLength(command);
        if (rcvName == "")
        {
            qDebug() << socketDescriptor << "Bad parameter!";
            disconnectfromHost();
        }
        QString msg = command;
        qDebug() << socketDescriptor
                 << UserName << "to" << rcvName << msg;

        emit messageToOne(msg, UserName, rcvName);
    }
    else
    {
        qDebug() << socketDescriptor << "Bad command!";
        disconnectfromHost();
    }
}


void NewClient::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    emit finished(this);
    deleteLater();
}


// Даем доступ или отказываем
bool NewClient::sendAccess(bool bAccess, QStringList names)
{
    if(socket->state() != QAbstractSocket::ConnectedState)
        return false;

    if (bAccess)
        qDebug() << socketDescriptor << "Welcome to chat " + TempName;
    else
        qDebug() << socketDescriptor << "Username " + TempName + " is already taken.";

    QByteArray barr;

    if(bAccess)
    {
        barr = prepareMessage("EVMp_WELCOME");
        socket->write(barr);

        QStringList::iterator i;
        for (i=names.begin(); i != names.end(); ++i) {
            barr = prepareMessage("EVMp_CONNECT_0_" +
                                  QString::number(i->length()) +
                                  "_" + *i);
            socket->write(barr);

        }
        UserName = TempName;
        return socket->waitForBytesWritten();
    }
    else
    {
        // TODO: более цивилизованный способ дать отказ
        QTimer::singleShot(100, this, &NewClient::disconnectfromHost);
        return false;
    }
}


bool NewClient::noticeConnect(QString name)
{
    if(socket->state() != QAbstractSocket::ConnectedState)
        return false;

    QByteArray barr = prepareMessage("EVMp_CONNECT_1_" +
                          QString::number(name.length()) +
                          "_" + name);
    socket->write(barr);
    return socket->waitForBytesWritten();
}

bool NewClient::noticeDisconnect(QString name)
{
    if(socket->state() != QAbstractSocket::ConnectedState)
        return false;

    QByteArray barr = prepareMessage("EVMp_DISCONNECT_" +
                          QString::number(name.length()) +
                          "_" + name);
    socket->write(barr);
    return socket->waitForBytesWritten();
}

// Сообщения для всех
bool NewClient::sendMessageToAll(QString msg, QString name)
{
    if(socket->state() != QAbstractSocket::ConnectedState)
        return false;

    QByteArray marr = prepareMessage(
        "EVMp_SENDMSG_" + QString::number(name.length()) +
        "_" + name + "_" + msg);
    socket->write(marr);
    return socket->waitForBytesWritten();
}


bool NewClient::sendMessageToOne(QString msg, QString name) //Приватные сообщения (можно объединить это с верхним через флаги, разница у них в посылаемой команде)
{
    if(socket->state() != QAbstractSocket::ConnectedState)
        return false;

    QByteArray parr = prepareMessage(
        "EVMp_PRIVATEMSG_" + QString::number(name.length()) +
        "_" + name + "_" + msg);
    socket->write(parr);
    return socket->waitForBytesWritten();
}


void NewClient::disconnectfromHost()
{
    socket->disconnectFromHost();
}
