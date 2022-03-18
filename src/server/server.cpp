#include <server.hpp>
#include <iostream>
#include <QThread>
#include <QDataStream>
#include <sstream>
#include <cstring>

Server::Server(QObject* parent) :
    QObject(parent)
{
    tcpServer = new QTcpServer(this);

    connect(tcpServer, SIGNAL(newConnection()),
            this, SLOT(acceptConnection())
    );

    if(! tcpServer -> listen(QHostAddress::Any, 8123))
        std::cout << "[ERR]: Bind failed!\n";
    else
        std::cout << "[INFO]: Server listening\n";
}

void Server::acceptConnection()
{
    clientSock = tcpServer->nextPendingConnection();
    connect(clientSock, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(clientSock, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
    clientSock->write(QByteArray::fromStdString(clientSock->peerAddress().toString().toStdString() + " connected to server !\n"));
    std::cout << "[INFO]: Client " << clientSock->peerAddress().toString().toStdString() << " connected!";
}

void Server::onReadyRead()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray datas = sender->readAll();
    clientSock->write(QByteArray::fromStdString(sender->peerAddress().toString().toStdString() + ": " + datas.toStdString()));

    packet* deserialized;
    deserialized = ( packet* ) datas.data();

    return;

}

void Server::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{   
    if (socketState == QAbstractSocket::UnconnectedState)
    {
        std::cout << "[INFO]: Client " << clientSock->peerAddress().toString().toStdString() << " disconnected!";
        clientSock->close();
        clientSock = nullptr;
    }
}