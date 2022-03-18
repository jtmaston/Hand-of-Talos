#include <QCoreApplication>
#include <QThread>
#include <server.hpp>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server robot_server;
    

    return a.exec();
}
