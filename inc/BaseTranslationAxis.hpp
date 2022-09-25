#pragma once

#include <QCoreApplication>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextStream>
#include "Logger.hpp"

class BaseTranslationAxis : public QObject
{
    Q_OBJECT
    public:
        BaseTranslationAxis();
        ~BaseTranslationAxis();
        void move(int range);
        int position;
        bool active;

    private:
        bool detect();
        
        QSerialPort port;
        bool moving;
    
    private slots:
        void read();
};