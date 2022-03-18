#pragma once
#include <Arm_lib.hpp>
#include <QObject>
#include <QTcpSocket>
#include <string>



class Client : public QObject
{
    Q_OBJECT
    public:
        explicit Client(QObject* parent = nullptr);
    //private:
        QTcpSocket* socket = nullptr;
    public slots:
        void onReadyRead();
    
};