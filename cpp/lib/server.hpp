#pragma once
#include <lib/libRobot/include/Arm_lib.hpp>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

class Server : public QObject
{
    Q_OBJECT
    public:
        explicit Server(QObject* parent = nullptr);
    private:
        void initServer();
        QTcpServer* tcpServer = nullptr;
    
    signals:

    public slots:
        void acceptConnection();
};