#include <iostream>
#include "Arm_lib.hpp"

using namespace std;

ArmDevice dev;

int main()
{
    uint16_t angles[] = {45, 45, 45, 45, 45, 45};
    //dev.servo_write6(angles, 2000);
    dev.servo_write(0, 90, 1000);
    return 0;
}
