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

        id += 0x10;

        uint8_t buf[] = {id, value_h, value_l, time_h, time_l};
        int t = write(this->bus, buf, 5); // TODO: add verification
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
            pos = (3700.0f - 380.0f) * (float)angles[i / 2 - 1] / 270.0f + 380.0f;
            break;
        default:
            //std::cout << "br3"<< '\n';
            pos = (3100.0f - 900.0f) * angles[i / 2 - 1] / 180.0f + 900.0f;
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
    t = write(this->bus, bytearr, 12); // TODO: verification
    usleep(time * 1000);
}

bool ArmDevice::send(uint8_t buffer[100], uint16_t buflen)
{
    if (write(this->bus, buffer, buflen) != 2)
    {
        return 0;
    }
    return 1;
}

int16_t ArmDevice::servo_read(uint8_t id)
{
    if (((id > 6) || (id < 1)) == 1)
    {
        UnmappedError err;
        throw err;
    }
    id += 0x30;
    uint8_t buf[3] = {id, 0};
    int pos = write(this->bus, buf, 2);
    usleep(3000);
    pos = i2c_smbus_read_word_data(this->bus, id); //read(this->bus, buf, 2);
    pos = (pos >> 8 & 0xff) | (pos << 8 & 0xff00);

    switch (pos)
    {
    case 5:
        pos = int(270 * (pos - 380) / (3700 - 380));
        break;
    default:
        pos = int(180 * (pos - 900) / (3100 - 900));
    }
    // TODO: verification
    if (id == 2 || id == 3 || id == 4)
        pos = 180 - pos;

    return pos;
}

int16_t *ArmDevice::servo_readall()
{
    static int16_t values[6];
    for (int i = 1; i <= 6; i++)
        values[i - 1] = this->servo_read(i);
    return values;
}

void ArmDevice::torque(bool state)
{
    uint8_t adj = state & 0xff;
    uint8_t buf[] = {this->addr, 0x1A, 100};
    int t = write(this->bus, buf, 3);
    std::cout << t;
}

void ArmDevice::neon_multiply(float32_t *T1, float32_t *T2, float32_t *T3)
{
    // Columns for T1
    float32x4_t T1_0;
    float32x4_t T1_1;
    float32x4_t T1_2;
    float32x4_t T1_3;

    // Columns for T2
    float32x4_t T2_0;
    float32x4_t T2_1;
    float32x4_t T2_2;
    float32x4_t T2_3;

    // Columns for T3
    float32x4_t T3_0;
    float32x4_t T3_1;
    float32x4_t T3_2;
    float32x4_t T3_3;

    // load values into vectors

    T1_0 = vld1q_f32(T1);
    T1_1 = vld1q_f32(T1 + 4);
    T1_2 = vld1q_f32(T1 + 8);
    T1_3 = vld1q_f32(T1 + 12);

    // set the result vector to be 0 filled

    T3_0 = vmovq_n_f32(0);
    T3_1 = vmovq_n_f32(0);
    T3_2 = vmovq_n_f32(0);
    T3_3 = vmovq_n_f32(0);

    T2_0 = vld1q_f32(T2);
    T3_0 = vfmaq_laneq_f32(T3_0, T1_0, T2_0, 0);
    T3_0 = vfmaq_laneq_f32(T3_0, T1_1, T2_0, 1);
    T3_0 = vfmaq_laneq_f32(T3_0, T1_2, T2_0, 2);
    T3_0 = vfmaq_laneq_f32(T3_0, T1_3, T2_0, 3);
    vst1q_f32(T3, T3_0);

    T2_1 = vld1q_f32(T2 + 4);
    T3_1 = vfmaq_laneq_f32(T3_1, T1_0, T2_1, 0);
    T3_1 = vfmaq_laneq_f32(T3_1, T1_1, T2_1, 1);
    T3_1 = vfmaq_laneq_f32(T3_1, T1_2, T2_1, 2);
    T3_1 = vfmaq_laneq_f32(T3_1, T1_3, T2_1, 3);
    vst1q_f32(T3 + 4, T3_1);

    T2_2 = vld1q_f32(T2 + 8);
    T3_2 = vfmaq_laneq_f32(T3_2, T1_0, T2_2, 0);
    T3_2 = vfmaq_laneq_f32(T3_2, T1_1, T2_2, 1);
    T3_2 = vfmaq_laneq_f32(T3_2, T1_2, T2_2, 2);
    T3_2 = vfmaq_laneq_f32(T3_2, T1_3, T2_2, 3);
    vst1q_f32(T3 + 8, T3_2);

    T2_3 = vld1q_f32(T3 + 12);
    T3_3 = vfmaq_laneq_f32(T3_3, T1_0, T2_3, 0);
    T3_3 = vfmaq_laneq_f32(T3_3, T1_1, T2_3, 1);
    T3_3 = vfmaq_laneq_f32(T3_3, T1_2, T2_3, 2);
    T3_3 = vfmaq_laneq_f32(T3_3, T1_3, T2_3, 3);
    vst1q_f32(T3 + 12, T3_3);
}

void ArmDevice::c_multiply(float32_t *A, float32_t *B, float32_t *C)
{
    int n = 4, m = 4, k = 4;
    for (int i_idx = 0; i_idx < n; i_idx++)
    {
        for (int j_idx = 0; j_idx < m; j_idx++)
        {
            C[n * j_idx + i_idx] = 0;
            for (int k_idx = 0; k_idx < k; k_idx++)
            {
                C[n * j_idx + i_idx] += A[n * k_idx + i_idx] * B[k * j_idx + k_idx];
            }
        }
    }
}