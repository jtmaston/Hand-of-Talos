cmake_minimum_required(VERSION 3.5)

project(RobotDashBoard VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_TYPE, Release)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -march=native -mtune=native -mcpu=native -Wno-narrowing")
set(CMAKE_CXX_FLAGS_RELEASE "-march=native -mtune=native -mcpu=native -Wall -Wpedantic")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

find_package( OpenCV REQUIRED )
include_directories( lib/opencv/include ${OpenCV_INCLUDE_DIRS})


set(TS_FILES extra/RobotDashBoard_en_US.ts)

if(UNIX AND NOT APPLE)
    add_library(libarm STATIC
        lib/libRobot/src/Arm_lib.cpp
    )
    set_target_properties(libarm PROPERTIES
        PUBLIC_HEADER "lib/libRobot/include/Arm_lib.hpp"
        ARCHIVE_OUTPUT_DIRECTORY "lib/libRobot/lib"
        LIBRARY_OUTPUT_DIRECTORY "lib/libRobot/lib"
    )
    target_include_directories(libarm PRIVATE lib/libRobot/include)
    target_compile_features(libarm
        PUBLIC
        cxx_attributes
        cxx_defaulted_functions
        cxx_deleted_functions
        cxx_final
    )

    #add_library(joystick STATIC
    #    lib/libJoystick/src/Joystick.cpp
    #)
    #set_target_properties(joystick PROPERTIES
    #    PUBLIC_HEADER "lib/libJoystick/include/Joystick.hpp"
    ##    ARCHIVE_OUTPUT_DIRECTORY "lib/libJoystick/lib"
     #   LIBRARY_OUTPUT_DIRECTORY "lib/libJoystick/lib"
    ##)
    #target_include_directories(joystick PRIVATE lib/libRobot/include)
    #target_compile_features(libarm
    #    PUBLIC
    #    cxx_attributes
    #    cxx_defaulted_functions
    #    cxx_deleted_functions
    #    cxx_final
    #)

endif()
set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        lib/mainwindow.h
        lib/mainwindow.ui
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

target_link_libraries(RobotDashBoard PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


if(UNIX AND NOT APPLE)
    target_link_libraries(RobotDashBoard PRIVATE libarm i2c)
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


message(STATUS "MY_CAMERA_LIBRARY: " ${opencv_selfbuild})
target_link_libraries(RobotDashBoard PRIVATE ${OpenCV_LIBS})

#target_link_libraries( RobotDashBoard PRIVATE  )

link_directories(../build/opencv/lib)
target_link_libraries(RobotDashBoard PUBLIC )


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RobotDashBoard)
endif()
