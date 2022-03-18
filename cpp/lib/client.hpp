#pragma once
#include <lib/libRobot/include/Arm_lib.hpp>
#include <QObject>
#include <QTcpSocket>

class Client : public QObject
{
    Q_OBJECT
    public:
        explicit Client(QObject* parent = nullptr);
    //private:
        QTcpSocket* socket = nullptr;
    
};