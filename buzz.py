from Arm_Lib import Arm_Device
from time import sleep

bot = Arm_Device()

bot.Arm_Buzzer_On()
sleep(1)
bot.Arm_Buzzer_Off()
