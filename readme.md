# The hand of Talos
## Description
The Hand of Talos is a simple program / framework designed to allow for control of a 5 DOF robotic arm. It is built around the DOFBot robot kit by Yahboom. It features a graphical interface, slider / incremental control for axes, a dedicated Learn Mode and much more. It's Open Source, Hackable, and ( soon ) fully documented and usable.

## How do I use this program?
The program was designed to run on either the Raspberry Pi or a Jetson Nano, as these platforms are the ones provided by the DOFBot kit. As such, it contains ARM64 specific code and instructions, as well as only being runnable under Linux. That being said, a brief explanation of how to actually *run* the program.

Once built, a RobotDashboard executable is created. Make sure i2c is enabled for the raspberry pi, then you're good to go! Run the program and watch it do its magic. The interface allows individual axis control ( by angles ), as well as different operating modes:
* Learn mode: this allows "direct learning" using the robot: you may manually put the robot in any pose you want, then save using the ">" button. Execution begins when the center button is pressed.
* Track object demos the robot's capability to track a colored object. Select the color you want to be tracked and watch it go! As a rule, it will track the largest object of that color
* Jog mode allows you to "jog" the robot, moving_ it manually through a controller. The control scheme *is* wonky, but in the future that will be changed. Pressed during the learn mode phase, this allows you to move the robot with a higher degree of control.
* Load Program: allows you to load compiled RASM binaries (.bin) into the interpreter, then begins program execution. 
* STOP : represents the emergency stop button, will halt all actions and disable torque on the motors. Note: if there is a load applied to the arm, it will fall. It is a known caveat, will be fixed sometime in the future. Industry behavior mandates locking of the brakes on the robot, however, evidently, there are no brakes on this structure

The right part represents the axis readout and direct kinematics calculation. That means 

## Releases
Releases are plug-and play. Simply download the program onto the Pi / Jetson, enable the I2C and camera interfaces, and you're good to go.

## Building from source
The project is still very WIP, and it may take a while for a release that actually works to be posted. Want to try it early? Everything is included in the repository. Just follow some simple steps:

    * Clone the repository and all submodules:
     ```git clone --recurse-submodules -j4 https://github.com/jtmaston/Hand-of-Talos.git```
    * Install the dependencies. This project depends on libi2c and qt. I'm assuming you're going to use a debian-based distro, if not, I'm sure you can figure out how to get the packages on your own
        - ```sudo apt-get install build-essential
            sudo apt-get install qtcreator
            sudo apt-get install qt5-default
            sudo apt-get install qttools5-dev```
        - ```sudo apt-get install libi2c-dev```
        - ```sudo apt-get install libopencv-dev```
        - ```sudo apt-get install cmake```
    * Make a build directory and build the program
        ``` mkdir build
            cd build
            cmake ../cpp/CMakeLists.txt
            cmake --build . ```
    * Have fun!
    * Note: it has been a while since this was updated. Additional modules may (?) need to be added, but you should find them by build errors.

## Python
This project started as a python-only endeavour, but due to performance issues on the raspi-4 I was using ( horrible, especially on the camera aspect ), the project has since been moved to C++. The Python code is still left up, for history's sake, feel free to also play around with it. The requirements *should* be in the requirements.txt, however no support will be provided for the py version.

## The Hand of Talos
What's with the name? Honestly, not much. It's a robotic arm, and the first robot that came up into my mind is Talos, the mythical Greek figure. ¯\\_(ツ)\_/¯

## Help!!! Something's broken!!!
Take a deep breath. It's okay. Open an issue. While I make no guarantees, I *might* fix it. This is a hobby project, not something designed to be production-grade.

## Licensing
This uses code from https://github.com/jtmaston/ArmLibCpp.git, a library I ported from python to cpp, licensed MIT to the original authors.