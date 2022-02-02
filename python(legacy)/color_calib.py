import threading
import cv2 as cv
from time import sleep
from dofbot_config   import *
import ipywidgets as   widgets
from IPython.display   import display
# Create and update   HSV instance
update_hsv =   update_hsv()
# Initialize the num   parameter
num=0
# Initialization mode
model =   "General"
# Initialize HSV name
HSV_name=None
# Initialize HSV value
color_hsv  = {"red"   : ((0, 43, 46), (10, 255, 255)),
              "green" : ((35, 43,   46), (77, 255, 255)),
              "blue"  : ((100, 43, 46), (124, 255, 255)),
              "yellow": ((26, 43,   46), (34, 255, 255))}
# Set random colors
color =   [[random.randint(0, 255) for _ in range(3)] for _ in range(255)]
# HSV parameter path
HSV_path="/home/jetson/dofbot_ws/src/dofbot_color_identify/scripts/HSV_config.txt"
# Read HSV   configuration file, update HSV value
try:   read_HSV(HSV_path,color_hsv)
except Exception:   print("Read HSV_config Error!!!")