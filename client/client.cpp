#include <lib/client.hpp>
#include <iostream>
#include <QThread>
#include <QDebug>
#include <QHostAddress>
#include <QAbstractSocket>
Client::Client(QObject* parent) :
    QObject(parent)
{
    socket = new QTcpSocket(this);
    socket -> connectToHost(QHostAddress::Any, 8123);
    if(socket->waitForConnected(3000))
        std::cout << "[INFO] Connection made!\n";
    else
        std::cout << "[INFO] Connection failed!\n";
}
void Client::onReadyRead()
{
    
}