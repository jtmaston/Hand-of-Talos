#pragma once

#include <QCoreApplication>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextStream>


class BaseTranslationAxis : public QObject
{
    Q_OBJECT
    public:
        BaseTranslationAxis();
        ~BaseTranslationAxis();
        void move(int range);
        int position;

    private:
        bool detect();
        bool active;
        QSerialPort port;
        bool moving;
    
    private slots:
        void read();
};