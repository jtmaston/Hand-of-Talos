#include <iostream>
#include "Arm_lib.hpp"

using namespace std;

ArmDevice dev;

void return_home()
{
    uint16_t target[] = {90, 90 ,90 ,0, 90, 90};
    dev.servo_write6(target, 2000);
}

void dance()
{
    //uint16_t angles[] = {45, 45, 45, 45, 45, 45};
    uint16_t angles[] = {45,45,45,45,45,45};
    dev.servo_write6(angles, 1500);

    uint16_t angles2[] = {0, 90, 0, 90, 90, 90};
    dev.servo_write6(angles2, 1500);
 
    uint16_t angles3[] = {90, 180, 0, 0, 90, 90};
    dev.servo_write6(angles3, 1500);
}

int main()
{
    //uint16_t angles[] = {45, 45, 45, 45, 45, 45};
    //dev.servo_write6(angles, 2000);
    //dev.servo_write(0, 90, 2000);
    //usleep(2100000);
    //dev.servo_read(1);
    //while (true){
    //    return_home();
    //    dance();
    //    usleep(1500000);
    //}
    //std::cout << dev.servo_read(1);
    /*uint16_t* values = dev.servo_readall();
    for(int i = 0; i < 6; i++)
        std::cout << values[i];*/

    dev.torque(0);

    return 0;
}
