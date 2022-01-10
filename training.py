from smbus2 import SMBus
from Arm_Lib import Arm_Device
from time import sleep

bot = Arm_Device()
sleep(0.01)

def reset():
    bot.Arm_serial_set_torque(1)
    bot.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500)      # reset position

def getPoint():
    pos = Pos()
    for i in range(0, 6):
        pos.points.append( bot.Arm_serial_servo_read(i + 1))
    return pos

class Pos():
    #def __init__(angle1, angle2, angle3, angle4, angle5, angle6):
    #    points = { angle1, angle2, angle3, angle4, angle5, angle6 }
    def __init__(self):
        self.points = list()


#if __name__ == "__main__":
#print("Entering testing mode: ")
#bot.Arm_serial_set_torque(0)
#a = input()


targets = list()

def run():
    done = False
    global targets
    bot.Arm_serial_set_torque(0)
    print("Please start moving the robot")
    print("Enter saves a step, q stops")
    while not done:
        c = input()
        if c == "q":
            done = True
        else:
            targets.append(getPoint())
    sleep(3)
    reset()
    print("Waiting a little bit")
    sleep(3)
    for i in targets:
        print(i.__dict__)
    for i in targets:
        bot.Arm_serial_servo_write6(
                i.points[0], i.points[1], i.points[2],
                i.points[3], i.points[4], i.points[5], 1000)
        sleep(2)
    print("Returning to position")
    sleep(3)
    reset()
if __name__ == "__main__":
    run()
