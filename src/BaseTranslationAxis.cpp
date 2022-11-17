#include <BaseTranslationAxis.hpp>
#include <iostream>
#include <QByteArray>

BaseTranslationAxis::BaseTranslationAxis()
{
    const auto serialPortInfos = QSerialPortInfo::availablePorts();

    active = false;
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
        if ( ( serialPortInfo.manufacturer().toStdString() == "1a86" ) && (serialPortInfo.productIdentifier() == 29987))
        {
            port.setBaudRate(QSerialPort::Baud9600);
            port.setPortName(serialPortInfo.portName());
            if ( port.open(QIODevice::ReadWrite) )
                active = true;
            break;
        }
            
    }

    if(!active)
    {
        std::cerr << "[ ERROR ]: Serial port init failed!";
    }

    connect(&port, &QSerialPort::readyRead, this, &BaseTranslationAxis::read);

    //return 0;
}

BaseTranslationAxis::~BaseTranslationAxis()
{
    port.close();
}

void BaseTranslationAxis::read()
{
    auto data = port.readAll();
    
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
    /**QByteArray data;
    data.append(std::to_string(range + 25).c_str());
    port.write(data);
    port.write("\n");
    position = range;
    moving = true;**/
}