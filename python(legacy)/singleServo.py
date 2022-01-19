from Arm_Lib import Arm_Device

bot = Arm_Device()

bot.Arm_serial_servo_write(5, 90, 1000)
