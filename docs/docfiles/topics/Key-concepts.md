# Key concepts

## Industrial robots
At its base level, an industrial robot comprises three key components: the mechanical structure, which is the
active part, directly involved in activities being realized, the controller, which houses all logic and power delivery
components, and the human-machine-interface (HMI), referred to as the “Teach-Pendant”. As the mechanical and controller
parts are part of the off-the-shelf robot kit, they will be considered as-is, and focus will be drawn to the HMI and
firmware.

## HMI's
The HMI achieves a number of goals: first of all, it must serve as a data link to the sensor array of the robot,
providing an operator with a constant feed of information, pertaining to the status of systems, the positions of the
robot axes (thereafter known as “joint angles”), the geometric position and orientation of the end-effector (referred
to as “cartesian coordinates”), the current program being followed and other optional features. A second goal is to
allow the operator to issue commands or modify already existing ones. This is done in what is called the “jogging
mode,” in which the operator has direct control over the mechanical structure, being able to freely move it,
to achieve simple trajectories and work paths. In this way, programs can be built quickly and efficiently.

## Machine vision
A given particularity of the robot kit utilized is that it is equipped with a camera, allowing for machine vision
applications. It was therefore decided that the HMI should also display a live camera feed, as well as real-time
graphics pertaining to the given application. To limit the scope of the demonstrative application, it was
chosen that the vision module would be utilized in color detection and recognition, being able to classify the three
base colors, Red, Green, and Blue.

## Firmware
With the front-facing functionality established, the backend must also be implemented. This is referred to as the
“firmware,” the code that the controller must run, to implement all functionality and drive the HMI.