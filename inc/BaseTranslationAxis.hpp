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
        ~BaseTranslationAxis() override;
        void move(int range);
        int position_{};
        bool active_;

    private:
        // bool detect();       TODO: maybe deprecate
        
        QSerialPort port_;
    bool moving_{};
    
    private slots:
        void read();
};