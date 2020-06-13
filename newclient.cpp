#include "newclient.h"
#include "qdatastream.h"
#include "QSqlDatabase"
#include "QSqlQuery"
#include "newserver.h"



NewClient::NewClient(qintptr ID, QObject *parent) :
    QObject(parent)
{

    socketDescriptor = ID;
    blockSize = 0;
//    QString dbName = QStringLiteral("myConnection_%1").arg(qintptr(QThread::currentThreadId()), 0, 16);
//        if(QSqlDatabase::contains(dbName))
//        {
//            db = QSqlDatabase::database(dbName);
//        }
//        else
//        {
//    db = QSqlDatabase::addDatabase("QMYSQL",dbName);
//    db.setHostName("localhost");
//    db.setDatabaseName("ChatDB");
//    db.setUserName("root");
//    db.setPassword("");
//    bool ok = db.open();
//        }
// if (!db.open())
//      qDebug() << "Not Connected";
// else
//      qDebug() << "Connected";


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
       // qDebug() << socketDescriptor << "_blockSize now " << _blockSize;
    }

    if (socket->bytesAvailable() < blockSize)
        return;
    else

        blockSize = 0;

    Action command;
    in >> command;
    qDebug() << socketDescriptor << "Received command " << command;

    //if (!_isAutched && command != comAutchReq)
       // return;

    switch(command)
    {

        case Connect:
        {


            QString name;
            in >> name;
            QString pass;
            in >> pass;

            qDebug() << socketDescriptor << "Recieve name" << name;
            qDebug() << socketDescriptor << "Recieve pass" << pass;

//            QSqlQuery *qry = new QSqlQuery(db);
//            qry->prepare("SELECT Username, Password FROM users WHERE Username = ?");
//            qry->addBindValue(name);
//            qry->exec();
//            qry->next();
//            QString uname = qry->value(0).toString();
//            QString passw = qry->value(1).toString();

//            qDebug() << socketDescriptor << "QRYRecieve name" << uname;
//            qDebug() << socketDescriptor << "QRYRecieve pass" << passw;

//            if (uname == name && pass == passw)
//                qDebug() << "Access Granted\n" << "Welcome back, " << name;
//            else
//                qDebug() <<"Access Denied";

            UserName = name;

            emit AddName(UserName, this);
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


        }


    }


}


void NewClient::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    emit finished(this);
    deleteLater();
}

//void NewThread::SetParent(NewServer *parserver)
//{

//    pserver = parserver;

//}

void NewClient::printName(bool bAccess, NewClient* client)
{
    if (client == this)
    {
        if (bAccess)
            qDebug() << socketDescriptor << "Welcome to chat " + UserName;
        else
            qDebug() << socketDescriptor << "Username " + UserName + " is already taken.";

        SendAccess(bAccess);

    }


}

bool NewClient::SendAccess(bool bAccess)
{
   if(socket->state() == QAbstractSocket::ConnectedState)
    {
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
            }

        socket->write(barr);
      //socket->flush();
        return socket->waitForBytesWritten();
      }
   else
       return false;


}

bool NewClient::UpdateNames(QMap<QString, NewClient*> names)
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
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
    else
        return false;
}

bool NewClient::SendMessageToAll(QString msg, QString name)
{

    if(socket->state() == QAbstractSocket::ConnectedState)
     {
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
    else
        return false;

}
