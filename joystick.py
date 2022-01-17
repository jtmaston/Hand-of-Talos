import pygame
from kivy import Logger

class spoof:
    def get_axis(self, axis):
        return 0

class Joystick:
    def __init__(self):
        self.init()
        pygame.init()

        self.buttons = ['up', 'down', 'left', 'right', 'start', 'A', 'B', 'X', 'Y', 'L', 'R']
        self.axes = [0, 0]

    def poll_buttons(self, caller=None):
        self.axes[0] = self.joystick.get_axis(0)
        self.axes[1] = self.joystick.get_axis(1)

    def init(self):
        try:
            joystick = pygame.joystick.Joystick(0)
            joystick.init()
            self.joystick = joystick
            self.joystick_name = joystick.get_name()
        except Exception:
            Logger.error("Joystick error! Spoofing!")
            self.joystick = spoof()
