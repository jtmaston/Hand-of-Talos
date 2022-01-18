from Arm_Lib import Arm_Device

dev = Arm_Device()

dev.Arm_serial_servo_write(0, 45, 3000)
