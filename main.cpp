#include <QCoreApplication>
#include "newserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NewServer server;
      server.startServer();

    return a.exec();
}
