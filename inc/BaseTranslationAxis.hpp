#pragma once

#include <QCoreApplication>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextStream>


class TranslationAxis : public QObject
{
    Q_OBJECT
    public:
        TranslationAxis();
        ~TranslationAxis() override;
        void move(int range);
        int position_;

    private:
        bool active_;
        QSerialPort port_;

    private slots:
        void read();
};