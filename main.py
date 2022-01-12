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

class DashboardApp(MDApp):
    def __init__(self):
        self.running = True
        super(DashboardApp, self).__init__()
        self.robot = Robot()
        Clock.schedule_interval(self.display_axis_data, 0)
        #Clock.schedule_interval(self.viewfinder, 0.033)
        self.motor_callback = Clock.schedule_interval(self.update_motors, 0)
        self.viewfinder_thread = Thread(target = self.viewfinder, args=(self,))
        #viewfinder_thread.start()

    def build(self, *args, **kwargs):
        super(DashboardApp, self).build(*args, **kwargs)
        ##self.viewfinder_thread.start()

    def viewfinder(self, *args, **kwargs):
        self = args[0]
        #camera = cv2.VideoCapture(0)

        """while self.running:
            print(self.running)
            ret, frame = camera.read()
            cv2.imshow("CV2 Image", frame)
            buf1 = cv2.flip(frame, 0)
            buf = buf1.tostring()
            texture1 = Texture.create(size=(frame.shape[1], frame.shape[0]), colorfmt='rgba')
            texture1.blit_buffer(buf, colorfmt='bgr', bufferfmt='ubyte')
            self.root.ids['camera'].texture = texture1

            print(self.root.ids['camera'].texture)

            sleep(0.1)"""
        self.root.ids['camera'].texture = self.robot.framebuffer()

    def display_axis_data(self, *args, **kwargs):
        print(self.root.ids['camera'].texture)
        angles = self.robot.poll_axes()
        for i in range(0, 5):
            self.root.ids[f'ax{i + 1}_readout'].text = f"Axa {i + 1}: {angles[i]}°"

    def update_motors(self, *args, **kwargs):
        self.robot.move([int(self.root.ids[f"ax{i + 1}"].value + 90) for i in range(0,6)])

    def learn(self, *args, **kwargs):
        #self.motor_callback.cancel()
        self.robot.reset()

    def on_stop(self, *args, **kwargs):
        self.runing = False

if __name__ == '__main__':
    DashboardApp().run()
