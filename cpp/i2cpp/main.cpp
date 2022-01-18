#include <iostream>
#include "Arm_lib.hpp"

using namespace std;

ArmDevice dev;

int main()
{
    dev.servo_write(0, 45, 3000);
  return 0;
}
