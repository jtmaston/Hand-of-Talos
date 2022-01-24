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
#include "linux/i2c-dev.h"
#include <i2c/smbus.h>
}

#define BLOCK_SIZE 4
#define __PI__ 3.14159265359
#define __RAD__ 0.0174533

class ArmDevice
{
    public:
        ArmDevice();
        void buzz(uint8_t time = 10);
        void noBuzz();
        void servo_write(uint8_t id, uint16_t angle, uint16_t time);
        void servo_write6(uint16_t angles[5], uint16_t time);
        void toggleTorque( bool torque );
        void rgb(uint8_t r, uint8_t g, uint8_t b);
        void reset_mcu();
        bool ping_servo(uint8_t id);
        void button_mode(bool mode);

        float32_t servo_read_any(uint8_t id);
        float32_t servo_read(uint8_t id);
        float32_t* servo_readall();

        void print_matrix(float32_t*);

        float angles[7] = {0, 0, 0, 0, 0, 0, 0};
        
        void neon_multiply(float32_t* T1, float32_t* T2, float32_t* T);
        void c_multiply(float32_t *A, float32_t *B, float32_t *C);

    private:
        const float translations[13] = {104.5,  70, 65, 247.5,  -130, 360,  238.5, 65,  141.5, 39.5, 25.5};
        int addr = 0x15;
        int bus;
        bool send(uint8_t bytearr[100], uint16_t numbytes);
        void rotateX(uint8_t num, float32_t* target );
        void rotateY(uint8_t num, float32_t* target);
        void rotateZ(uint8_t num, float32_t* target);

        void translateX(uint8_t num, float32_t* target );
        void translateY(uint8_t num, float32_t* target);
        void translateZ(uint8_t num, float32_t* target);
        void calculate_end_effector(float32_t* target);

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

