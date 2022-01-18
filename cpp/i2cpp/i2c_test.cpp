#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), creat() */

#include <chrono>
#include <thread>


extern "C"
{
#include<linux/i2c-dev.h>
#include <i2c/smbus.h>
}


int main()
{
  int addr = 0x15;

  char filename[30];
  snprintf(filename, 19, "/dev/i2c-%d", 1);
  int file = open(filename, O_RDWR);
  if (file < 0) {
    exit(1);
  }

  if (ioctl(file, I2C_SLAVE, addr) < 0) {
    exit(1);
  }



  __u8 reg = 0x01;
  __s32 res;
  char buf[10];

  buf[0] = reg;
  buf[1] = 0x01;
  //buf[2] = 0x65;
  if (write(file, buf, 2) != 2) {
    /* ERROR HANDLING: i2c transaction failed */
    std::cout << "WRITE_FAIL";
  }

  res = i2c_smbus_read_byte_data(file, 0x01);
  if (res < 0) {
    std::cout << "FAIL";
  } else {
    std::cout << res;
  }

  buf[0] = 0x06;
  buf[1] = 0x14;
  write(file, buf, 2);

  return 0;


}
