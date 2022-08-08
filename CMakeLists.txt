cmake_minimum_required(VERSION 3.5)

project(RobotDashboard VERSION 0.1 LANGUAGES CXX C)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -march=native -mtune=native -Wno-narrowing -Wno-trigraphs -no-pie")
set(CMAKE_CXX_FLAGS_RELEASE "-march=native -mtune=native -Wall -Wpedantic -O3 -Wno-trigraphs -no-pie")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Concurrent Network Gamepad SerialPort  REQUIRED)

find_package( OpenCV REQUIRED )

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

    ${PROJECT_SOURCE_DIR}/src/assets/noSignal.cpp

    ${PROJECT_SOURCE_DIR}/src/init/peripherals.cpp
    ${PROJECT_SOURCE_DIR}/src/init/signals.cpp
    ${PROJECT_SOURCE_DIR}/src/init/device.cpp

    ${PROJECT_SOURCE_DIR}/src/modules/ui.cpp
    ${PROJECT_SOURCE_DIR}/src/modules/camera.cpp
    ${PROJECT_SOURCE_DIR}/src/modules/BaseTranslationAxis.cpp
    ${PROJECT_SOURCE_DIR}/src/modules/motion.cpp
    ${PROJECT_SOURCE_DIR}/src/modules/RASM_Interpreter.cpp

    ${PROJECT_SOURCE_DIR}/src/operation_modes/tracking_mode.cpp
    ${PROJECT_SOURCE_DIR}/src/operation_modes/jog_mode.cpp
    ${PROJECT_SOURCE_DIR}/src/operation_modes/learn_mode.cpp
    ${PROJECT_SOURCE_DIR}/src/operation_modes/program_mode.cpp
    
)

set(HEADERS
    ${PROJECT_SOURCE_DIR}/inc/mainwindow.hpp
    ${PROJECT_SOURCE_DIR}/inc/mainwindow.ui
    ${PROJECT_SOURCE_DIR}/inc/BaseTranslationAxis.hpp
    ${PROJECT_SOURCE_DIR}/inc/Image.hpp

    ${PROJECT_SOURCE_DIR}/lib/libRobot/include/Arm_lib.hpp
    ${PROJECT_SOURCE_DIR}/lib/libRobot/include/Arm_exc.hpp

    ${PROJECT_SOURCE_DIR}/lib/libRobotExtension/RobotArm.cpp
    ${PROJECT_SOURCE_DIR}/lib/libRobotExtension/RobotArm.hpp

    ${PROJECT_SOURCE_DIR}/lib/libRASM/inc/Instruction.hpp
    ${PROJECT_SOURCE_DIR}/lib/libRASM/inc/isa.hpp
    ${PROJECT_SOURCE_DIR}/lib/libRASM/inc/Variable.hpp
    
)

set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/lib/libRobotExtension
    ${PROJECT_SOURCE_DIR}/lib/libRASM/inc
    ${PROJECT_SOURCE_DIR}/lib/libRobot/include
    ${PROJECT_SOURCE_DIR}/lib
    ${PROJECT_SOURCE_DIR}/lib/libQuirc/lib
    ${PROJECT_SOURCE_DIR}/inc
)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${INCLUDE_DIRS})
set(CMAKE_AUTOMOC_SEARCH_PATHS ${INCLUDE_DIRS})
set(CMAKE_AUTORCC_SEARCH_PATHS ${INCLUDE_DIRS})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

    qt_add_executable(RobotDashboard
        MANUAL_FINALIZATION
        ${CLIENT_SOURCES}
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

else()
    if(ANDROID)
        add_library(RobotDashboard SHARED
            ${CLIENT_SOURCES}
        )

    else()
        add_executable(RobotDashboard
            ${SOURCES} ${HEADERS}
        )

        target_include_directories( RobotDashboard
            PUBLIC
            ${INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS}
        )

    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(RobotDashboard PUBLIC Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Concurrent Qt${QT_VERSION_MAJOR}::Gamepad Qt${QT_VERSION_MAJOR}::SerialPort)
target_link_libraries(RobotDashboard PUBLIC 5DOF i2c)
target_link_libraries(RobotDashboard PUBLIC quirc ${OpenCV_LIBS} )

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RobotDashboard)
endif()
