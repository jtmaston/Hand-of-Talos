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
#include <arm_neon.h>

extern "C"
{
#include<linux/i2c-dev.h>
#include <i2c/smbus.h>
}

#define BLOCK_SIZE 4

class ArmDevice
{
    public:
        ArmDevice();
        void buzz(uint8_t time = 10);
        void noBuzz();
        void servo_write(uint8_t id, uint16_t angle, uint16_t time);
        void servo_write6(uint16_t angles[5], uint16_t time);
        int16_t servo_read(uint8_t id);
        int16_t* servo_readall();
        void torque(bool state);

        void neon_multiply(float32_t* T1, float32_t* T2, float32_t* T);
        void c_multiply(float32_t *A, float32_t *B, float32_t *C);

    private:
        uint8_t addr = 0x15;
        int8_t bus;
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
    ERROR HANDLING: i2c transaction failed 
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

