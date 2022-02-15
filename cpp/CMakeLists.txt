cmake_minimum_required(VERSION 3.5)

project(RobotDashBoard VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_TYPEextra/RobotDashBoard_en_US.ts, Release)
set_property(SOURCE lib/libjoystick/joystick.hh PROPERTY SKIP_AUTOGEN ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -march=native -mtune=native -mcpu=native -Wno-narrowing")
set(CMAKE_CXX_FLAGS_RELEASE "-march=native -mtune=native -mcpu=native -Wall -Wpedantic -O3")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Concurrent REQUIRED)

find_package( OpenCV REQUIRED )


set(TS_FILES extra/RobotDashBoard_en_US.ts extra/RobotDashBoard_ro_ro.ts)

qt5_create_translation(TS_FILES)

add_subdirectory(lib/libRobot)

add_library(joystick STATIC
    lib/libjoystick/joystick.cc
)
set_target_properties(joystick PROPERTIES
    PUBLIC_HEADER "lib/libjoystick/joystick.hh"
)
target_include_directories(joystick PRIVATE /include)
target_compile_features(joystick
    PUBLIC
    cxx_attributes
    cxx_defaulted_functions
    cxx_deleted_functions
    cxx_final
)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        lib/mainwindow.h
        lib/mainwindow.ui
        lib/RobotArm.hpp
        src/RobotArm.cpp
        src/RASM_Interpreter.cpp
        lib/libRASM/Assembler/inc/Instruction.hpp
        lib/libRASM/Assembler/inc/isa.hpp
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RobotDashBoard
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RobotDashBoard APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(RobotDashBoard SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RobotDashBoard
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(RobotDashBoard PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Concurrent)


if(UNIX AND NOT APPLE)
    target_link_libraries(RobotDashBoard PRIVATE 5DOF i2c)
endif()
if(APPLE)
    set_target_properties(RobotDashBoard PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )
endif()

target_link_libraries(RobotDashBoard PRIVATE ${OpenCV_LIBS} joystick)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RobotDashBoard)
endif()
