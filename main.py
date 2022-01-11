from math import floor

from kivy.clock import Clock

from kivy import Config

Config.set('graphics', 'width', f'{1080}')
Config.set('graphics', 'height', f'{720}')
Config.set('graphics', 'maxfps', '60')

import kivy
from kivymd.app import MDApp

kivy.require('2.0.0')
from Arm_Lib import Arm_Device

angles = [30, 90, 71, 24, 24]


class DashboardApp(MDApp):
    def __init__(self):
        super(DashboardApp, self).__init__()
        #self.robot = Arm_Device()
        Clock.schedule_interval(self.spoof_axis_data, 0)
        Clock.schedule_interval(self.poll_axis_data, 0)

    def build(self):
        pass

    def get_axis_data(self):
        for i in range(0, 6):
            self.root.ids[f'ax{i + 1}'] = self.robot.Arm_serial_servo_read(i + 1)

    def poll_axis_data(self, *args, **kwargs):
        global angles
        angles.clear()
        for i in range(0, 5):
            angles.append(int(floor(self.root.ids[f"ax{i + 1}"].value)))

    def spoof_axis_data(self, *args, **kwargs):
        for i in range(0, 5):
            self.root.ids[f'ax{i + 1}_readout'].text = f"Axa {i + 1}: {angles[i]}°"


if __name__ == '__main__':
    DashboardApp().run()
