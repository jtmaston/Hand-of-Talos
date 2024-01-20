.
├── CMakeLists.txt
├── dirStruct.md
├── docs
│   └── Writerside
│       ├── cfg
│       │   └── buildprofiles.xml
│       ├── hotd.tree
│       ├── images
│       │   └── dofbot.png
│       ├── topics
│       │   ├── Goals.md
│       │   ├── High-level.md
│       │   ├── Key-concepts.md
│       │   ├── Overview.md
│       │   └── The-main-program-loop.md
│       └── writerside.cfg
├── extra
│   ├── python(legacy).zip
│   ├── RobotDashBoard_en_US.ts
│   └── RobotDashBoard_ro_ro.ts
├── firmware
│   └── firmware.ino
├── inc
│   ├── BaseTranslationAxis.hpp
│   ├── mainwindow.hpp
│   ├── mainwindow.ui
│   └── noSignal.hpp
├── lib
│   ├── libQuirc
│   │   ├── demo
│   │   │   ├── camera.c
│   │   │   ├── camera.h
│   │   │   ├── convert.c
│   │   │   ├── convert.h
│   │   │   ├── demo.c
│   │   │   ├── demo_opencv.cxx
│   │   │   ├── demoutil.c
│   │   │   ├── demoutil.h
│   │   │   ├── dthash.c
│   │   │   ├── dthash.h
│   │   │   ├── mjpeg.c
│   │   │   ├── mjpeg.h
│   │   │   └── scanner.c
│   │   ├── lib
│   │   │   ├── decode.c
│   │   │   ├── identify.c
│   │   │   ├── quirc.c
│   │   │   ├── quirc.h
│   │   │   ├── quirc_internal.h
│   │   │   └── version_db.c
│   │   ├── LICENSE
│   │   ├── Makefile
│   │   ├── README.md
│   │   └── tests
│   │       ├── dbgutil.c
│   │       ├── dbgutil.h
│   │       ├── inspect.c
│   │       ├── inspect_opencv.cxx
│   │       └── qrtest.c
│   ├── libRASM
│   │   ├── CMakeLists.txt
│   │   ├── inc
│   │   │   ├── Instruction.hpp
│   │   │   ├── isa.hpp
│   │   │   ├── QR-Code-generator
│   │   │   │   ├── c
│   │   │   │   │   ├── Makefile
│   │   │   │   │   ├── qrcodegen.c
│   │   │   │   │   ├── qrcodegen-demo.c
│   │   │   │   │   ├── qrcodegen.h
│   │   │   │   │   ├── qrcodegen-test.c
│   │   │   │   │   └── Readme.markdown
│   │   │   │   ├── cpp
│   │   │   │   │   ├── Makefile
│   │   │   │   │   ├── qrcodegen.cpp
│   │   │   │   │   ├── QrCodeGeneratorDemo.cpp
│   │   │   │   │   ├── qrcodegen.hpp
│   │   │   │   │   └── Readme.markdown
│   │   │   │   ├── java
│   │   │   │   │   ├── pom.xml
│   │   │   │   │   ├── QrCodeGeneratorDemo.java
│   │   │   │   │   ├── Readme.markdown
│   │   │   │   │   └── src
│   │   │   │   │       └── main
│   │   │   │   │           └── java
│   │   │   │   │               ├── io
│   │   │   │   │               │   └── nayuki
│   │   │   │   │               │       └── qrcodegen
│   │   │   │   │               │           ├── BitBuffer.java
│   │   │   │   │               │           ├── DataTooLongException.java
│   │   │   │   │               │           ├── package-info.java
│   │   │   │   │               │           ├── QrCode.java
│   │   │   │   │               │           ├── QrSegmentAdvanced.java
│   │   │   │   │               │           └── QrSegment.java
│   │   │   │   │               └── module-info.java
│   │   │   │   ├── java-fast
│   │   │   │   │   ├── io
│   │   │   │   │   │   └── nayuki
│   │   │   │   │   │       └── fastqrcodegen
│   │   │   │   │   │           ├── BitBuffer.java
│   │   │   │   │   │           ├── DataTooLongException.java
│   │   │   │   │   │           ├── Memoizer.java
│   │   │   │   │   │           ├── package-info.java
│   │   │   │   │   │           ├── QrCodeGeneratorDemo.java
│   │   │   │   │   │           ├── QrCode.java
│   │   │   │   │   │           ├── QrSegmentAdvanced.java
│   │   │   │   │   │           ├── QrSegment.java
│   │   │   │   │   │           ├── QrTemplate.java
│   │   │   │   │   │           └── ReedSolomonGenerator.java
│   │   │   │   │   └── Readme.markdown
│   │   │   │   ├── python
│   │   │   │   │   ├── qrcodegen-demo.py
│   │   │   │   │   ├── qrcodegen.py
│   │   │   │   │   ├── Readme.markdown
│   │   │   │   │   └── setup.py
│   │   │   │   ├── Readme.markdown
│   │   │   │   ├── rust
│   │   │   │   │   ├── Cargo.toml
│   │   │   │   │   ├── examples
│   │   │   │   │   │   └── qrcodegen-demo.rs
│   │   │   │   │   ├── Readme.markdown
│   │   │   │   │   └── src
│   │   │   │   │       └── lib.rs
│   │   │   │   ├── rust-no-heap
│   │   │   │   │   ├── Cargo.toml
│   │   │   │   │   ├── examples
│   │   │   │   │   │   └── qrcodegen-demo.rs
│   │   │   │   │   ├── Readme.markdown
│   │   │   │   │   └── src
│   │   │   │   │       └── lib.rs
│   │   │   │   └── typescript-javascript
│   │   │   │       ├── build.sh
│   │   │   │       ├── qrcodegen-input-demo.html
│   │   │   │       ├── qrcodegen-input-demo.ts
│   │   │   │       ├── qrcodegen-output-demo.html
│   │   │   │       ├── qrcodegen-output-demo.ts
│   │   │   │       ├── qrcodegen.ts
│   │   │   │       └── Readme.markdown
│   │   │   └── Variable.hpp
│   │   ├── RASM Examples
│   │   │   ├── if.rasm
│   │   │   ├── multiple_servos.rasm
│   │   │   ├── paint.rasm
│   │   │   ├── pickandplace.rasm
│   │   │   ├── target.rasm
│   │   │   ├── up_down.rasm
│   │   │   └── variable.rasm
│   │   ├── README.md
│   │   └── src
│   │       └── assembler.cpp
│   └── libRobot
│       ├── CMakeLists.txt
│       ├── include
│       │   └── Arm_lib.hpp
│       ├── LICENSE
│       └── src
│           └── Arm_lib.cpp
├── LICENSE
├── programs
│   ├── pickAndPlace2.rasm
│   ├── PickNPlace.bin
│   ├── PickNPlace.jpg
│   ├── Prog1.bin
│   └── Prog1.jpg
├── readme.md
└── src
    ├── init
    │   ├── camera.cpp
    │   └── setup.cpp
    ├── main.cpp
    ├── mainwindow.cpp
    └── modules
        ├── camera.cpp
        ├── jog.cpp
        ├── joystick.cpp
        ├── libRobotExtension
        │   ├── RobotArm.cpp
        │   └── RobotArm.hpp
        ├── logger
        │   ├── Logger.cpp
        │   └── Logger.hpp
        ├── menuIdentifier.hpp
        ├── motion.cpp
        ├── RASM.cpp
        ├── teachIn.cpp
        ├── translationAxis.cpp
        └── userInterface.cpp

49 directories, 139 files
