import random

from Arm_Lib import Arm_Device
from time import sleep, time
import cv2
from kivy.graphics.texture import Texture
from kivy import Logger

class spoof:
    def Arm_serial_set_torque(self, tq):
        pass

    def Arm_serial_servo_read(self, id):
        return random.randint(-90, 90)

    def Arm_serial_set_torque(self, tq):
        pass
    def Arm_serial_servo_write6_array(self, arr, time):
        pass

class Robot:
    def __init__(self):

        try:
            self.arm = Arm_Device()
        except Exception:
            self.arm = spoof()

        sleep(0.01)
        self.camera = cv2.VideoCapture(0)
        self.steps = []
        self.camera = None
        self.disable_move = False
        self.done = True
        self.axes = [0, 0, 0, 0, 0, 0]
        self.alive = True

    def halt(self):
        self.arm.Arm_serial_set_torque(10)
        self.arm.done = True

    def poll_axes(self):
        time_started = time()
        while self.alive:
            #delta = time() - time_started
            #if  delta > 1:
            #    Logger.error("Timeout in poll, expect I2C bus fail!")
            #    print(delta)     FIXME!
            angles = [self.arm.Arm_serial_servo_read(i + 1) for i in range(0, 6)]
            self.axes = [i if i is not None else 0 for i in angles]
            sleep(0.1)

    def move(self, target_angles):
        if not self.disable_move:
            if target_angles[-1] < 25:
                target_angles[-1] = 25
            self.arm.Arm_serial_servo_write6_array(target_angles, 500)
            return target_angles

    def reset(self):
        self.arm.Arm_serial_set_torque(1)
        self.arm.Arm_serial_servo_write6_array([90, 90, 90, 0, 90, 90], 500)
        sleep(2)

    def toggle_learn(self):
        self.disable_move = True
        self.reset()
        self.arm.Arm_serial_set_torque(10)  # disable torque

    def add_step(self):
        self.steps.append(self.poll_axes())

    def remove_step(self):
        try:
            self.steps.pop()
        except IndexError:
            return False

        return True

    def execute(self):
        # self.disable_move = True
        self.done = False
        self.arm.Arm_serial_set_torque(1)
        while self.alive:
            for i in self.steps:
                if i is not None:
                    self.arm.Arm_serial_servo_write6_array(i, 2000)
                    sleep(2)
