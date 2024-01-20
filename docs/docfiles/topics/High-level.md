# High-level

At a high level, the program comprises the following components:
- **The main program loop**: This dictates the sequence in which each component is accessed and commanded. The main loop
handles interfacing with the operator, the render thread (drawing to the screen).
- **Camera loop**: This handles interfacing to the camera, and graphical processing algorithms (in the case of the 
application described, color recognition and tracking). In order to increase performance, it is offloaded to a separate
thread, running in parallel with the main loop.
- **Teach-in loop**: Handles learning of points via direct learning. This has been offloaded to a separate thread to try
and reduce load on the main program loop, since it tended to cause lag in the interface update code.
- **Input thread**: input handling (controller button presses)

- **Motor control**: The robot is equipped with a coprocessor that acts as a man in the middle between the raspberry
pi and the motors of the mechanical unit. The command sequence was reverse-engineered by observing the demo application
that came with the robot, sniffing
[i2c](https://www.ti.com/lit/an/slva704/slva704.pdf?ts=1705720344435&ref_url=https%253A%252F%252Fwww.google.com%252F)
commands, and disassembling the [python library](https://pypi.org/project/Arm_lib/) provided by Yahboom.
- **Kinematics**: The implementation of direct kinematics routines, used in calculating position and orientation of the
end-effector.
- **Program interpreter**: The implementation of an instruction set and matching interpreter, used in order to ensure
the programmability of the robot.
- **External axis**: The program has the functionality of connecting an external translation axis to the robot, and
synchronizing its movements with the rest of the robot axes.