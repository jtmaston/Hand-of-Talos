#include <QCoreApplication>
#include <QThread>
#include <lib/client.hpp>
#include <QString>
#include <QDataStream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Client robot_client;
    QDataStream output;
    output.setDevice(robot_client.socket);
    output.setVersion(QDataStream::Qt_5_9);


    while( true )
    {
        QString message = "hello, betch!\n";
        output.startTransaction();
        output << "Hello!\n";
        output.commitTransaction();
    }

    return a.exec();
}
