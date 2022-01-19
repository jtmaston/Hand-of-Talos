#include "Arm_lib.hpp"
#include <iostream>

ArmDevice::ArmDevice()
{
    BusError_C BusError;
    this->bus = open("/dev/i2c-1", O_RDWR);
    this->addr = 0x15;
    if (this->bus < 0)
    {
        throw BusError;
    }

    if (ioctl(this->bus, I2C_SLAVE, this->addr) < 0)
    {
        throw BusError;
    }
}

void ArmDevice::buzz(uint8_t time)
{
    time = time & 0xff;
    uint8_t buf[] = {0x06, time};
    send(buf, 2);
}

void ArmDevice::noBuzz()
{
    uint8_t buf[] = {0x06, 0};
    send(buf, 2);
}

void ArmDevice::servo_write(uint8_t id, uint16_t angle, uint16_t time)
{
    switch (id)
    {
    case 0:
    {
        uint16_t angles[] = {angle, angle, angle, angle, angle, angle};
        this->servo_write6(angles, time);
        break;
    }

    default:
    {
        uint8_t value_h, value_l, time_h, time_l;
        uint16_t pos;

        //std::cout << pos;

        switch (id)
        {
        case 2:
        case 3:
        case 4:
            angle = 180 - angle;
            pos = int((3100 - 900) * (angle - 0) / (180 - 0) + 900);
            std::cout << "Branch 1";
            break;

        case 5:
            pos = int((3700 - 380) * (angle - 0) / (270 - 0) + 380);
            std::cout << "Branch 2";
            break;

        default:
            std::cout << "Default";
            pos = int((3100 - 900) * (angle - 0) / (180 - 0) + 900);
            break;
        }

        value_h = (pos >> 8) & 0xFF;
        value_l = pos & 0xFF;

        time_h = (time >> 8) & 0xFF;
        time_l = time & 0xFF;

        id = id + 0x10;

        uint8_t buf[] = {id, value_h, value_l, time_h, time_l};
        int t = write(this->bus, buf, 5);
        break;
    }
    }
}

void ArmDevice::servo_write6(uint16_t angles[6], uint16_t time)
{
    uint8_t bytearr[14] = {0};
    bytearr[0] = 0x1D;
    uint8_t timearr[3];
    timearr[0] = 0x1E;

    bool flag = 0;
    for (uint8_t i = 0; i < 6; i++)
    {
        flag |= ((angles[i] > 180) || (angles[i] < 0));
        //std::cout << angles[i] << " ";
    }

    if (flag)
    {
        AngleError_C err;
        throw err;
    }

    for (uint8_t i = 2; i < 13; i += 2)
    {
        //std::cout << (int)angles[i / 2 - 1] << " ";
        float pos = 0;
        switch (i / 2)
        {
        case 2:
        case 3:
        case 4:
        {
            //std::cout << "br1" << '\n';
            uint16_t angle = 180 - angles[i / 2 - 1];
            pos = ((3100.0f - 900.0f) * (float)angle / 180.0f + 900.0f);
            break;
        }
        case 5:
            //std::cout << "br2"<< '\n';
            pos = (3700.0f - 380.0f) * (float)angles[i / 2 - 1] / 270.0f  + 380.0f;
            break;
        default:
            //std::cout << "br3"<< '\n';
            pos = (3100.0f - 900.0f) * angles[i / 2 - 1]/ 180.0f + 900.0f;
            break;
        }
        uint16_t p_adj = trunc(pos);
        bytearr[i - 1] = (p_adj >> 8) & 0xFF;
        bytearr[i] = p_adj & 0xFF;
    }
    //std::cout << '\n';

    timearr[1] = (time >> 8) & 0xFF;
    timearr[2] = time & 0xFF;

    //for (int i = 1; i < 13; i++)
        //std::cout << (int)bytearr[i] << " ";

    //std::cout << '\n';

    //std::cout << (int)timearr[1] << " " << (int)timearr[2] << '\n';

    int t = write(this->bus, timearr, 3);
    t = write(this->bus, bytearr, 12);
}

bool ArmDevice::send(uint8_t buffer[100], uint16_t buflen)
{
    if (write(this->bus, buffer, buflen) != 2)
    {
        return 0;
    }
    return 1;
}
