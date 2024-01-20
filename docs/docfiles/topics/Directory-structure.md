# Directory structure

The project's directory structure may seem a little confusing at first, since it has suffered multiple modifications
over the years. This page aims to explain how files are organized and laid out. This page is non-exhaustive; it will not
pinpoint individual files unless they are relevant for understanding the layout.

As a general convention, **directories** will be marked with bold, `files` with italics.

- **Top directory**
  - `CMakeLists.txt`: contains the build sequence and targets to be built
  - `.gitignore`: contains the files to be ignored (not uploaded) onto GitHub: usually build artifacts and local test 
  files.
  - `LICENSE`: contains the license file
  - `.gitmodules`: contains the external git repositories linked to this one.
  - **src**: contains source files (`.cpp`, `.hpp`) of each part of the project
    - **init**: contains initialization code for the robot
      - `camera.cpp`: setup functions for the video capture and QR decoder
      - `setup.cpp`: setup functions for the user interface elements and timers used
    - **modules**: contains the components of the program, as 'modules,' pairs of `.cpp` and `.hpp` files, usually with
    a matching name.
      - **libRobotExtension**: contains the implementation of the kinematics module, as well as some rebindings of the
      libRobot functionality. It derives its name from the fact that I intended to keep libRobot as close as possible to
      its Python counterpart (written by Yahboom).
      - **logger**: contains bindings to functions that provide basic logging functionality
      - `RASM.cpp`: contains interpreter code for executing robot code
      - `camera.cpp`: contains bindings to functions that provide camera failure handling (e.g., disconnect) and the
      capture and postprocessing functions.
      - `jog.cpp`: contains bindings to the main 'jogging' function, which parses data from the user interface to cause
      the robot to move to a given joint position.
      - `joystick.cpp`: contains bindings to the joystick handling function
      - `menuIdentifier.hpp`: legacy code, will be deprecated
      - `motion.cpp`: contains bindings to functions that communicate with the coprocessor and controls robot motion
      - `teachIn.cpp`: contains bindings to functions that allow the robot to learn the positions in which it is placed
      - `translationAxis`: contains bindings to the function that interfaces with the external robot's translation axis.
      - `userInterface`: contains the code for buttons and other graphical elements that are accessible to the user.
    - **firmware**: contains the arduino code for the external translation axis code
    - **inc**: contains headers used in the code. A bit of a legacy part, since it could be refactored into **src**.
      - `BaseTranslationAxis.hpp`: header for external axis bindings
      - `mainwindow.hpp`: header for the application's main loop's objects and functions
      - `mainwindow.ui`: graphical component, editable using QT Designer, that describes where each graphical element is
      located in the main window.
      - `noSingnal.hpp`: a bitmap of a 'no signal' image, converted into an embeddable C++ object. This was done so the
      executable could be self-contained, and not have danging `.jpg` files around it.
    - **lib**: contains external dependencies, both written by myself but placed in individual repos and external ones.
      - **libQuirc**: library for decoding QR codes, you may find more info [here](https://github.com/dlbeer/quirc)
      - **libRASM**: implementation of the RASM interpreter and programming language, more info 
      [here](https://github.com/jtmaston/RASM)
      - **libRobot**: reverse-engineered port of the default Yahboom library, described
      [here](https://pypi.org/project/Arm_lib/)
    - **programs**: contains a few demo RASM programs which I have used throughout the years. Some are hand-written,
    some have been exported from RoboDK. Your mileage may vary.
