from Arm_Lib import Arm_Device
from time import sleep
import cv2
from kivy.graphics.texture import Texture
from kivy.clock import Clock

class Robot:
    def __init__(self):
        self.arm = Arm_Device()
        sleep(0.01)
        self.camera = cv2.VideoCapture(0)
        self.steps = []
        self.camera = None

    def framebuffer(self):
        ret, frame = self.camera.read()
        cv2.imshow("CV2 Image", frame)
        buf1 = cv2.flip(frame, 0)
        buf = buf1.tostring()
        texture1 = Texture.create(size=(frame.shape[1], frame.shape[0]), colorfmt='rgba')
        texture1.blit_buffer(buf, colorfmt='bgr', bufferfmt='ubyte')
        return texture1

    def poll_axes(self):
        angles = [self.arm.Arm_serial_servo_read(i + 1) for i in range(0, 5)]
        return [i if i is not None else 0 for i in angles]

    def move(self, target_angles):
        if target_angles[-1] < 25:
            target_angles[-1] = 25
        #print(target_angles)
        self.arm.Arm_serial_servo_write6_array(target_angles, 500)

    def reset(self, callback = None):
        self.arm.Arm_serial_set_torque(1)
        self.arm.Arm_serial_servo_write6(90, 90, 90, 0, 90, 90, 500)      # reset position
        #sleep(1)
        if callback is not None:
            callback.cancel()

    def mode_train(self):
        self.reset()

        global x
        done = False
        global targets
        self.arm.Arm_serial_set_torque(10)
        print("Please start moving the robot")
        print("Enter saves a step, q stops")

        while not done:
            print(f"Step {x}: ", end="")
            x += 1
            c = input()
            if c == "q":
                print("Robot will move to default position! Stand clear!")
                done = True
            else:
                targets.append(getPoint(bot))
        sleep(3)

        print(targets)

        while True:
            self.reset()
            print("Waiting a little bit")
            sleep(3)
            for i in targets:
                self.arm.Arm_serial_servo_write6_array(i, 1000)
                sleep(2)
                print("Returning to position")
                sleep(3)
