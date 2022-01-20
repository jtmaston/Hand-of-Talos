#include <iostream>
#include "Arm_lib.hpp"

using namespace std;

ArmDevice dev;

void return_home()
{
    uint16_t target[] = {90, 90, 90, 0, 90, 90};
    dev.servo_write6(target, 2000);
}

void dance()
{
    //uint16_t angles[] = {45, 45, 45, 45, 45, 45};
    uint16_t angles[] = {45, 45, 45, 45, 45, 45};
    dev.servo_write6(angles, 1500);

    uint16_t angles2[] = {0, 90, 0, 90, 90, 90};
    dev.servo_write6(angles2, 1500);

    uint16_t angles3[] = {90, 180, 0, 0, 90, 90};
    dev.servo_write6(angles3, 1500);
}

void init(float32_t* m)
{
    for(int i = 0 ; i < 4; i++)
    {
        for(int j = 0 ; j < 4; j++)
        {
            m[ j * 4 + i ] = (float) j / 175.0f + 1;
        }
    }
}

int main()
{
    float32_t end_effector [ 16 ];
    dev.angles[0] = 45;
    dev.angles[4] = 45;
    dev.calculate_end_effector(end_effector);
    dev.print_matrix(end_effector);

    //dev.c_multiply(a, b, c);

    

    return 0;
}
