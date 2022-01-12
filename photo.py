from cv2 import *

cam = VideoCapture(0)

result, image = cam.read()
if result:
	imwrite("test.png", image)
else:
	print("Camera fail!")
