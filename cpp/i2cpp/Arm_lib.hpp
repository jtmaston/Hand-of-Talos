#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), creat() */

#include "./Arm_exc.hpp"

#include <chrono>
#include <thread>

#include <cmath>

extern "C"
{
#include<linux/i2c-dev.h>
#include <i2c/smbus.h>
}

class ArmDevice
{
    public:
        ArmDevice();
        void buzz(uint8_t time = 10);
        void noBuzz();
        void servo_write(uint8_t id, uint16_t angle, uint16_t time);
        void servo_write6(uint16_t angles[5], uint16_t time);

    private:
        int addr = 0x15;
        int bus;
        bool send(uint8_t bytearr[100], uint16_t numbytes);
};

/*int main()
{


  __u8 reg = 0x01;
  __s32 res;
  char buf[10];

  buf[0] = reg;
  buf[1] = 0x01;
  //buf[2] = 0x65;
  if (write(file, buf, 2) != 2) {
    /* ERROR HANDLING: i2c transaction failed 
    std::cout << "WRITE_FAIL";
  }

  res = i2c_smbus_read_byte_data(file, 0x01);
  if (res < 0) {
    std::cout << "FAIL";
  } else {
    std::cout << res;
  }

  

  return 0;


}
*/

