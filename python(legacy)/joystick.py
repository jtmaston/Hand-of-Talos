import pygame
from kivy import Logger

class spoof:
    def get_axis(self, axis):
        return 0
    def get_button(self, *args, **kwargs):
        return 0;
    def get_numbuttons(self, *args, **kwargs):
        return 0;

class Joystick:
    def __init__(self):
        self.init()
        pygame.init()

        self.buttons = ['up', 'down', 'left', 'right', 'start', 'A', 'B', 'X', 'Y', 'L', 'R']
        self.axes = [0, 0, 0, 0, 0, 0]

    def poll_buttons(self, caller=None):
        self.axes[0] = self.joystick.get_axis(0)
        self.axes[1] = self.joystick.get_axis(1)
        self.axes[2] = self.joystick.get_axis(2)
        self.axes[3] = self.joystick.get_axis(3)
        self.axes[4] = -1 if self.joystick.get_button(0) == True else ( 1 if self.joystick.get_button(2) == True else False)
        self.axes[5] = -1 if self.joystick.get_button(4) == True else ( 1 if self.joystick.get_button(5) == True else False)


        num = self.joystick.get_numbuttons()

        buttons = []
        for i in range(num):
            buttons.append(self.joystick.get_button(i))

    def init(self):
        try:
            joystick = pygame.joystick.Joystick(0)
            joystick.init()
            self.joystick = joystick
            self.joystick_name = joystick.get_name()
        except Exception:
            Logger.error("Joystick error! Spoofing!")
            self.joystick = spoof()
