#include "newthread.h"
#include "qdatastream.h"
#include "QSqlDatabase"
#include "QSqlQuery"
#include "newserver.h"

NewThread::NewThread(qintptr ID, QObject *parent) :
    QThread(parent)
{
    QMutexLocker locker(&mutex);
    this->socketDescriptor = ID;
    blockSize = 0;
    QString dbName = QStringLiteral("myConnection_%1").arg(qintptr(QThread::currentThreadId()), 0, 16);
        if(QSqlDatabase::contains(dbName))
        {
            db = QSqlDatabase::database(dbName);
        }
        else
        {
    db = QSqlDatabase::addDatabase("QMYSQL",dbName);
    db.setHostName("localhost");
    db.setDatabaseName("ChatDB");
    db.setUserName("root");
    db.setPassword("");
    bool ok = db.open();
        }
 if (!db.open())
      qDebug() << "Not Connected";
 else
      qDebug() << "Connected";




}



void NewThread::run()
{
    // thread starts here
    qDebug() << " Thread started";

    mutex.lock();
    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // We'll have multiple clients, we want to know which is which
    qDebug() << socketDescriptor << " Client connected";

    // make this thread a loop,
    // thread will stay alive so that signal/slot to function properly
    // not dropped out in the middle when thread dies
    mutex.unlock();
    //exec();

    while(!quit)
    {


    }
}


void NewThread::readyRead()
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

        case Verify:
        {


            QString name;
            in >> name;
            QString pass;
            in >> pass;

            qDebug() << socketDescriptor << "Recieve name" << name;
            qDebug() << socketDescriptor << "Recieve pass" << pass;

            QSqlQuery *qry = new QSqlQuery(db);
            qry->prepare("SELECT Username, Password FROM users WHERE Username = ?");
            qry->addBindValue(name);
            qry->exec();
            qry->next();
            QString uname = qry->value(0).toString();
            QString passw = qry->value(1).toString();

            qDebug() << socketDescriptor << "QRYRecieve name" << uname;
            qDebug() << socketDescriptor << "QRYRecieve pass" << passw;

           // if (uname == name && pass == passw)
            //    qDebug() << "Access Granted\n" << "Welcome back, " << name;
           // else
            //    qDebug() <<"Access Denied";

            qint64 intnew = socketDescriptor;
            UserName = name;
            //connect(this, &NewThread::AddName, pserver, &NewServer::SlotAddName);
            //pserver->AddNewName(UserName);
            //pserver->debugPrintNames();

            emit AddName(UserName, intnew);
            //SendAccess(true);

//
        }
        break;

    }

    //for (long long i = 0; i < 4000000000; ++i){}
}


void NewThread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";


    socket->deleteLater();
    exit(0);
}

//void NewThread::SetParent(NewServer *parserver)
//{

//    pserver = parserver;

//}

void NewThread::printName(bool bAccess, qint64 ID)
{
    if (socketDescriptor == ID)
    {
        if (bAccess)
            qDebug() << socketDescriptor << "Welcome to chat " + UserName;
        else
            qDebug() << socketDescriptor << "Username " + UserName + " is already taken.";

        SendAccess(bAccess);

    }


}

void NewThread::SendAccess(bool bAccess)
{

    QByteArray barr;
    QDataStream stream (&barr, QIODevice::WriteOnly);
    Action ac = Action::Access;
   // stream << quint16(0);
    stream << ac;
    stream << bAccess;
    qDebug() << bAccess;
    //stream.device()->seek(0);
    //socket->moveToThread(this);
    socket->write(barr);
    socket->waitForBytesWritten();



}

void NewThread::UpdateNames(QMap<QString, qint64> names)
{
    QMutexLocker locker(&mutex);
    NamesMap = names;
    Action ac = NameUpdate;
    QByteArray arr;
    QDataStream stream (&arr, QIODevice::WriteOnly);
    QList<QString> newnames;
    QMap<QString, qint64>::iterator i;
    for (i=NamesMap.begin(); i != NamesMap.end(); ++i)
        newnames << i.key();
    stream << ac;
    stream << newnames;
   // socket->moveToThread(this);
    socket->write(arr);
    socket->waitForBytesWritten();


}
