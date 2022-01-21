set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_USE_STATIC_STD_LIBS TRUE)
set(BUILD_SHARED_LIBS FALSE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++1z -march=native -mtune=native -mcpu=native")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -std=c++1z -march=native -mtune=native -mcpu=native -ftree-vectorize")

cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0003 NEW)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_VER "0.0.1")

project(DOFBot  VERSION "${PROJECT_VER}"
    LANGUAGES CXX C
)

find_package(Qt5 COMPONENTS Core Widgets DBus Multimedia MultimediaWidgets REQUIRED)
#find_package(Qt5Multimedia REQUIRED)


# << ARMLib >>
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
    
    #RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_executable(DOFBot
    src/main.cpp
    #src/neontest.cpp
)

find_package(Qt5OpenGL REQUIRED)
  set(plugins Qt5::OpenGL)

#target_include_directories(DOFBot PRIVATE lib/imgui)
#target_include_directories(DOFBot PRIVATE lib/imgui/backends)
#target_include_directories(DOFBot PRIVATE lib/imgui-sfml)
target_include_directories(DOFBot PRIVATE lib/libRobot/include)

#target_link_libraries(DOFBot PUBLIC imgui)
#target_link_libraries(DOFBot PUBLIC sfml-graphics sfml-audio)
target_link_libraries(DOFBot PUBLIC Qt5::Widgets Qt5::DBus Qt5::Multimedia Qt5::MultimediaWidgets)
target_link_libraries(DOFBot PUBLIC libarm)
target_link_libraries(DOFBot PUBLIC i2c)
#target_link_libraries(DOFBot PUBLIC imgui-sfml)
