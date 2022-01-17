import cv2

from os import environ

environ["KIVY_CAMERA"] = 'opencv'
environ['KIVY_WINDOW'] = 'sdl2'
environ['KIVY_VIDEO'] = 'ffmpeg'
environ['KIVY_IMAGE'] = 'sdl2'

from kivy import Config

Config.set('graphics', 'width', f'{1080}')
Config.set('graphics', 'height', f'{720}')
Config.set('graphics', 'maxfps', '60')

from kivymd.app import MDApp

from kivy.graphics.texture import Texture

import kivy

from robot import Robot
from joystick import Joystick
from kivy.clock import Clock

from threading import Thread
from numpy import clip
from ctypes import *

ik_lib = CDLL("ik.so")


class Point(Structure):
    _fields_ = [
        ('x', c_float),
        ('y', c_float),
        ('z', c_float)
    ]


def PytoC(arr):
    seq = c_int * len(arr)
    return seq(*arr)


class DashboardApp(MDApp):
    def __init__(self):
        super(DashboardApp, self).__init__()
        self.robot = Robot()

        Clock.schedule_interval(self.display_axis_data, 0.1)
        Clock.schedule_interval(self.viewfinder, 0.066)
        Clock.schedule_interval(self.update_motors, 0.05)

        self.control_pad = Joystick()
        Clock.schedule_interval(self.control_pad.poll_buttons, 0.1)
        self.slider_cache = []

        # self.camera = cv2.VideoCapture(0)
        self.robot.arm.Arm_serial_set_torque(1)
        # self.camTex = Texture.create(size=(640, 480), colorfmt='rgba')

    def build(self, *args, **kwargs):
        super(DashboardApp, self).build(*args, **kwargs)
        Thread(target=self.robot.poll_axes).start()

    def viewfinder(self, *args, **kwargs):
        # ret, frame = self.camera.read()
        # cv2.imshow("CV2 Image", frame)
        ##buf1 = cv2.flip(frame, 0)
        # buf = buf1.tobytes()
        # self.camTex.blit_buffer(buf, colorfmt='bgr', bufferfmt='ubyte')
        # self.root.ids['camera'].texture = self.camTex
        pass

    def control(self):
        self.control_pad.poll_buttons()

        self.root.ids['ax1'].value -= round(self.control_pad.axes[0] * 100) / 0.5
        self.root.ids['ax4'].value += round(self.control_pad.axes[1] * 100) / 0.5
        restrict = clip([self.root.ids[f'ax{i + 1}'].value for i in range(0, 6)], -90, 90)

        for num, val in enumerate(restrict):
            self.root.ids[f'ax{num + 1}'].value = int(round(val))

    def display_axis_data(self, *args, **kwargs):
        angles = self.robot.axes
        for i in range(0, 5):
            self.root.ids[f'ax{i + 1}_readout'].text = f"Axa {i + 1}: {angles[i]}°"

    def update_motors(self, *args, **kwargs):
        self.control()
        targets = [int(self.root.ids[f"ax{i + 1}"].value + 90) for i in range(0, 6)]
        self.robot.move(targets)

        tgt = Point.from_address( ik_lib.inverse_kinematics(PytoC(targets), PytoC([1, 1, 1, 1, 1])) )
        print(f"{tgt.x} {tgt.y} {tgt.z}")
        ik_lib.free_point(byref(tgt))

    def mde_lrn(self):
        Thread(target=self.robot.toggle_learn).start()

    def add(self):
        Thread(target=self.robot.add_step).start()
        self.root.ids['stepcount'].text = str(int(self.root.ids['stepcount'].text) + 1)

    def rm(self):
        exit_code = self.robot.remove_step()
        if exit_code:
            self.root.ids['stepcount'].text = str(int(self.root.ids['stepcount'].text) - 1)

    def strt(self):
        Thread(target=self.robot.execute).start()

    def on_stop(self):
        self.robot.alive = False


if __name__ == '__main__':
    DashboardApp().run()
