# Overview

## Description

This project presents the implementation of industrial robot functionality, applied to a low-cost, educational 5-axis
articulated robot arm. The goal was to ensure the key aspects of an industrial robot: programmability, constant control
and constant human-machine interaction and feedback. In this sense, the software created replaces a traditional
teach-pendant, while offering its functionality.

Programmability is achieved by utilizing a custom instruction set, designed to offer students a simplified version of
traditional robot programming languages, and easily implemented in postprocessors. A constant data link is
established with the robot’s controller, with information about axis positions and error states (such as collisions,
hardware alarms) being displayed to the user.

Kinematic calculations are done by using a custom implementation, leveraging the functionality of the controller’s
AARCH64 core: by parallelization of matrix multiplications using SIMD instructions, calculation times are decreased by
a factor of 10x compared to the traditional iterative method. The software also implements “play-back” or “teach-in”
functionality, allowing the human operator to guide the robot to given positions and record steps, allowing for fast
prototype programming.

## Documentation map
The documentation is split into two components: the high-level part, explaining concepts without going into detail,
and a low-level part that details exactly how everything was implemented in code. You may navigate to these through
the left-side bar.