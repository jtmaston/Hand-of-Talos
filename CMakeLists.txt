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

set(CMAKE_CXX_FLAGS_DEBUG "-g -march=native -mtune=native -Wno-narrowing")
set(CMAKE_CXX_FLAGS_RELEASE "-march=native -mtune=native -Wall -Wpedantic -O3")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Concurrent Network Gamepad SerialPort  REQUIRED)

find_package( OpenCV REQUIRED )


set(TS_FILES extra/RobotDashBoard_en_US.ts extra/RobotDashBoard_ro_ro.ts)

qt5_create_translation(TS_FILES)

add_subdirectory(lib/libRobot)
add_subdirectory(lib/libRASM)


set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/RASM_Interpreter.cpp
    ${PROJECT_SOURCE_DIR}/src/mainwindow.cpp
    ${PROJECT_SOURCE_DIR}/src/BaseTranslationAxis.cpp
)

set(HEADERS
    ${PROJECT_SOURCE_DIR}/lib/mainwindow.h
    ${PROJECT_SOURCE_DIR}/lib/mainwindow.ui
    ${PROJECT_SOURCE_DIR}/lib/libRobotExtension/RobotArm.cpp
    ${PROJECT_SOURCE_DIR}/lib/libRobotExtension/RobotArm.hpp
    ${PROJECT_SOURCE_DIR}/lib/libRobot/include/Arm_lib.hpp
    ${PROJECT_SOURCE_DIR}/lib/libRobot/include/Arm_exc.hpp
    ${PROJECT_SOURCE_DIR}/lib/libRASM/Assembler/inc/Instruction.hpp
    ${PROJECT_SOURCE_DIR}/lib/libRASM/Assembler/inc/isa.hpp
    ${PROJECT_SOURCE_DIR}/lib/libRASM/Assembler/inc/Variable.hpp
    ${PROJECT_SOURCE_DIR}/lib/BaseTranslationAxis.hpp
    ${PROJECT_SOURCE_DIR}/lib/noSignal.hpp
)

set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/lib/libRobotExtension
    ${PROJECT_SOURCE_DIR}/lib/libRASM/Assembler/inc
    ${PROJECT_SOURCE_DIR}/lib/libRobot/include
    ${PROJECT_SOURCE_DIR}/lib
)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${INCLUDE_DIRS})
set(CMAKE_AUTOMOC_SEARCH_PATHS ${INCLUDE_DIRS})
set(CMAKE_AUTORCC_SEARCH_PATHS ${INCLUDE_DIRS})

#include_directories(
#    ${INCLUDES}
#    ${OpenCV_INCLUDE_DIRS}
#)

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

if(UNIX AND NOT APPLE)
    target_link_libraries(RobotDashboard PUBLIC 5DOF i2c)
endif()
if(APPLE)
    set_target_properties(RobotDashboard PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.jtmaston.site
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )
endif()

target_link_libraries(RobotDashboard PUBLIC ${OpenCV_LIBS})


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RobotDashboard)
endif()
