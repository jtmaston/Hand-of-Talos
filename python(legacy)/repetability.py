from Arm_Lib import Arm_Device
from time import sleep

#try:
bot = Arm_Device()
def home():
    #bot.Arm_serial_servo_write(6, 150, 1000)
    bot.Arm_serial_servo_write6(90, 45, 50, 45, 90, 180, 3000)
#
#    bot.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 1000)

def left():
    #bot.Arm_serial_servo_write(1, 90, 3000)
    while True:
        bot.Arm_serial_servo_write(1, 70, 3000)
        sleep(3.1)
        bot.Arm_serial_servo_write(1, 110, 3000)
        sleep(3.1)
#left()
while True:
    home()
    sleep(3.1)
    bot.Arm_serial_servo_write6(90, 90, 90, 0, 90, 180, 1500)
    sleep(1.6)
