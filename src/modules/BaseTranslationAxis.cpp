#include <BaseTranslationAxis.hpp>
#include <iostream>
#include <QByteArray>

BaseTranslationAxis::BaseTranslationAxis()
{
    const auto serial_port_infos = QSerialPortInfo::availablePorts();

    active_ = false;
    for (const QSerialPortInfo &serial_port_info : serial_port_infos) {
        if ((serial_port_info.manufacturer().toStdString() == "1a86" ) && (serial_port_info.productIdentifier() == 29987))
        {
            port_.setBaudRate(QSerialPort::Baud9600);
            port_.setPortName(serial_port_info.portName());
            if ( port_.open(QIODevice::ReadWrite) )
                active_ = true;
            break;
        }
            
    }

    if(!active_)
    {
        Logger::warning("Cannot find base translation axis. Control is disabled!");
    }else
    {
        connect(&port_, &QSerialPort::readyRead, this, &BaseTranslationAxis::read);
    }
    position_ = 0;
    moving_ = false;
    

    //return 0;
}

BaseTranslationAxis::~BaseTranslationAxis()
{
    port_.close();
}

void BaseTranslationAxis::read()
{
    auto data = port_.readAll();
    
    switch(data[0])
    {
        case 'd':
        {
            std::cout << "Done\n";
            break;
        }
        case 'r':
        {
            std::cout << "Ready\n";
            break;
        }
        case 's':
        {
            std::cout << "Software limit reached!\n";
            break;
        }
        case 'h':
        {
            std::cout << "Hardware limit reached!\n";
            break;
        }

        case '0':
        {break;}

        default:
        {
            std::cout << data.toStdString();
            break;
        }
    } 
}

void BaseTranslationAxis::move(int range)
{
    QByteArray data;
    data.append(std::to_string(range + 25).c_str());
    port_.write(data);
    port_.write("\n");
    position_ = range;
    moving_ = true;
}