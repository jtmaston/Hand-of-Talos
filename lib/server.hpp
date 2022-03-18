#pragma once
#include <Arm_lib.hpp>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <string>

struct packet
{
    uint8_t command;
    uint16_t data1;
    uint16_t data2;
    uint16_t data3;

};

class Server : public QObject
{
    Q_OBJECT
    public:
        explicit Server(QObject* parent = nullptr);
        virtual ~Server(){};
    private:
        void initServer();
        QTcpServer* tcpServer = nullptr;
        QTcpSocket* clientSock = nullptr;
    
    signals:

    public slots:
        void acceptConnection();
        void onSocketStateChanged(QAbstractSocket::SocketState socketState);
        void onReadyRead();
};