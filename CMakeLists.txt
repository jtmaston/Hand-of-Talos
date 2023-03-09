cmake_minimum_required(VERSION 3.5)

project(RobotDashboard VERSION 0.1 LANGUAGES CXX C)

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g  -mtune=native -Wno-narrowing -Wno-trigraphs")
set(CMAKE_CXX_FLAGS_RELEASE " -mtune=native -Wall -Wpedantic -O3 -Wno-trigraphs")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Concurrent Network Gamepad SerialPort REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(OpenCV REQUIRED)

add_library(
        quirc
        STATIC
        ${PROJECT_SOURCE_DIR}/lib/libQuirc/lib/decode.c ${PROJECT_SOURCE_DIR}/lib/libQuirc/lib/identify.c ${PROJECT_SOURCE_DIR}/lib/libQuirc/lib/quirc.c ${PROJECT_SOURCE_DIR}/lib/libQuirc/lib/version_db.c
)

set(TS_FILES extra/RobotDashBoard_en_US.ts extra/RobotDashBoard_ro_ro.ts)

qt5_create_translation(TS_FILES)

add_subdirectory(lib/libRobot)
add_subdirectory(lib/libRASM)

set(SOURCES
        ${PROJECT_SOURCE_DIR}/src/main.cpp
        ${PROJECT_SOURCE_DIR}/src/mainwindow.cpp
        ${PROJECT_SOURCE_DIR}/src/modules/translationAxis.cpp
        ${PROJECT_SOURCE_DIR}/src/modules/joystick.cpp
        ${PROJECT_SOURCE_DIR}/src/modules/camera.cpp
        ${PROJECT_SOURCE_DIR}/src/modules/userInterface.cpp
        ${PROJECT_SOURCE_DIR}/src/modules/motion.cpp
        ${PROJECT_SOURCE_DIR}/src/modules/teachIn.cpp
        ${PROJECT_SOURCE_DIR}/src/modules/jog.cpp
        ${PROJECT_SOURCE_DIR}/src/modules/RASM.cpp
        ${PROJECT_SOURCE_DIR}/src/init/setup.cpp
        ${PROJECT_SOURCE_DIR}/src/init/camera.cpp
        ${PROJECT_SOURCE_DIR}/src/modules/libRobotExtension/RobotArm.cpp
        ${PROJECT_SOURCE_DIR}/src/modules/logger/Logger.cpp
        )

set(HEADERS
        ${PROJECT_SOURCE_DIR}/src/modules/menuIdentifier.hpp
        ${PROJECT_SOURCE_DIR}/src/modules/logger/Logger.hpp
        ${PROJECT_SOURCE_DIR}/inc/mainwindow.ui
        ${PROJECT_SOURCE_DIR}/inc/mainwindow.hpp
        ${PROJECT_SOURCE_DIR}/src/modules/libRobotExtension/RobotArm.hpp
        ${PROJECT_SOURCE_DIR}/lib/libRobot/include/Arm_lib.hpp
        ${PROJECT_SOURCE_DIR}/lib/libRASM/inc/Instruction.hpp
        ${PROJECT_SOURCE_DIR}/lib/libRASM/inc/isa.hpp
        ${PROJECT_SOURCE_DIR}/lib/libRASM/inc/Variable.hpp
        ${PROJECT_SOURCE_DIR}/inc/BaseTranslationAxis.hpp
        ${PROJECT_SOURCE_DIR}/inc/noSignal.hpp

        )

set(INCLUDE_DIRS
        ${PROJECT_SOURCE_DIR}/src/modules/
        ${PROJECT_SOURCE_DIR}/src/modules/libRobotExtension
        ${PROJECT_SOURCE_DIR}/src/modules/logger
        ${PROJECT_SOURCE_DIR}/lib/libRASM/inc
        ${PROJECT_SOURCE_DIR}/lib/libRobot/include
        ${PROJECT_SOURCE_DIR}/lib
        ${PROJECT_SOURCE_DIR}/lib/libQuirc/lib
        ${PROJECT_SOURCE_DIR}/inc
        )

set(CMAKE_AUTOUIC_SEARCH_PATHS ${INCLUDE_DIRS})
set(CMAKE_AUTOMOC_SEARCH_PATHS ${INCLUDE_DIRS})
set(CMAKE_AUTORCC_SEARCH_PATHS ${INCLUDE_DIRS})


#if(ANDROID)
#    add_library(RobotDashboard SHARED
#        ${CLIENT_SOURCES}
#    )

#else()


add_executable(RobotDashboard
        ${HEADERS} ${SOURCES}
        )

target_include_directories(RobotDashboard
        PUBLIC ${OpenCV_INCLUDE_DIRS} ${INCLUDE_DIRS}
        )

#endif()


target_link_libraries(RobotDashboard PUBLIC Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Concurrent Qt${QT_VERSION_MAJOR}::Gamepad Qt${QT_VERSION_MAJOR}::SerialPort)
target_link_libraries(RobotDashboard PUBLIC 5DOF i2c)
target_link_libraries(RobotDashboard PUBLIC quirc ${OpenCV_LIBS})

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RobotDashboard)
endif ()
