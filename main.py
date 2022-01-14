import cv2

from math import floor
from kivy import Config

Config.set('graphics', 'width', f'{1080}')
Config.set('graphics', 'height', f'{720}')
Config.set('graphics', 'maxfps', '60')

from kivymd.app import MDApp

from kivy.clock import Clock
from kivy.graphics.texture import Texture

import kivy

from robot import Robot
from kivy.clock import Clock
from kivy import Config

from time import sleep

from threading import Thread
import asynckivy as ak

class DashboardApp(MDApp):
    def __init__(self):
        super(DashboardApp, self).__init__()
        self.robot = Robot()

        Clock.schedule_interval(self.display_axis_data, 0.1)
        Clock.schedule_interval(self.viewfinder, 0.066)
        Clock.schedule_interval(self.update_motors, 0.1)
        #Clock.schedule_interval(self.upd, 0.066)

        self.camera = cv2.VideoCapture(0)
        self.robot.arm.Arm_serial_set_torque(1)
        self.camTex = Texture.create(size=(640, 480), colorfmt='rgba')

    def build(self, *args, **kwargs):
        super(DashboardApp, self).build(*args, **kwargs)
        Thread(target=self.robot.poll_axes).start()

    def viewfinder(self, *args, **kwargs):
        ret, frame = self.camera.read()
        cv2.imshow("CV2 Image", frame)
        buf1 = cv2.flip(frame, 0)
        buf = buf1.tobytes()
        #texture1 = 
        self.camTex.blit_buffer(buf, colorfmt='bgr', bufferfmt='ubyte')
        self.root.ids['camera'].texture = self.camTex
        #Thread(target=self.blit, args=(self.camTex, buf, )).start()


    def upd( self, pp ):
        #print(pp)
        self.root.ids['camera'].texture = self.camTex

    @staticmethod
    def blit(tgt_tex, buf):
        #print(tgt_tex)
        tgt_tex.blit_buffer(buf, colorfmt='bgr', bufferfmt='ubyte')

    def display_axis_data(self, *args, **kwargs):
        angles = self.robot.axes
        for i in range(0, 5):
            self.root.ids[f'ax{i + 1}_readout'].text = f"Axa {i + 1}: {angles[i]}°"

    def update_motors(self, *args, **kwargs):
        self.robot.move([int(self.root.ids[f"ax{i + 1}"].value + 90) for i in range(0,6)])
        #Thread(target=self.robot.move, args=([int(self.root.ids[f"ax{i + 1}"].value + 90) for i in range(0,6)], )).start()

    def mde_lrn(self):
        Thread(target=self.robot.toggle_learn).start()

    def add(self):
        self.robot.add_step()

    def rm(self):
        self.robot.remove_step()

    def strt(self):
        Thread(target=self.robot.execute).start()

    def on_stop(self):
        self.robot.alive = False

if __name__ == '__main__':
    DashboardApp().run()
