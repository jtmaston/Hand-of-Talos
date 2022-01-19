set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_USE_STATIC_STD_LIBS TRUE)
set(BUILD_SHARED_LIBS FALSE)

cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0003 NEW)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_VER "0.0.1")

project(DOFBot  VERSION "${PROJECT_VER}"
    LANGUAGES CXX C
)

#find_package(OpenGL REQUIRED)

# << SFML >>
#add_subdirectory(lib/sfml)

# << JSONCPP >>

#add_library(jsoncpp STATIC 
#  lib/jsoncpp/src/lib_json/json_reader.cpp lib/jsoncpp/src/lib_json/json_value.cpp lib/jsoncpp/src/lib_json/json_value.cpp lib/jsoncpp/src/lib_json/json_valueiterator.inl lib/jsoncpp/src/lib_json/json_writer.cpp
#)
#set_target_properties(jsoncpp PROPERTIES PUBLIC_HEADER lib/jsoncpp/include/json.h)
#target_include_directories(jsoncpp PRIVATE lib/jsoncpp/include)

#target_compile_features(jsoncpp
##    PUBLIC
 #   cxx_attributes
#    cxx_defaulted_functions
#    cxx_deleted_functions
#    cxx_final
#)

# << IMGUI >>
#add_library(imgui STATIC 
#  lib/imgui/imgui.cpp lib/imgui/imgui_widgets.cpp lib/imgui/imgui_tables.cpp lib/imgui/imgui_draw.cpp
#)
#set_target_properties(imgui PROPERTIES PUBLIC_HEADER lib/imgui/imgui.h)
#target_include_directories(imgui PRIVATE lib/imgui)
#target_compile_features(imgui 
##    PUBLIC
 #   cxx_attributes
 #   cxx_defaulted_functions
 #   cxx_deleted_functions
 #   cxx_final
#)
# << >>

# << imgui-sfml >>

#add_library(imgui-sfml STATIC 
#  lib/imgui-sfml/imgui-SFML.cpp
#)
#set_target_properties(imgui-sfml PROPERTIES PUBLIC_HEADER lib/imgui-sfml/imgui-SFML.h)
#target_include_directories(imgui-sfml PRIVATE lib/imgui-sfml lib/imgui)

#target_compile_features(imgui-sfml
#    PUBLIC
#    cxx_attributes
##    cxx_defaulted_functions
 #   cxx_deleted_functions
#    cxx_final
#)

#target_link_libraries(imgui-sfml
#  PUBLIC
#    sfml-graphics
#    sfml-system
#    sfml-window
#    ${OPENGL_LIBRARIES}
#)

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
)

#target_include_directories(DOFBot PRIVATE lib/imgui)
#target_include_directories(DOFBot PRIVATE lib/imgui/backends)
#target_include_directories(DOFBot PRIVATE lib/imgui-sfml)
target_include_directories(DOFBot PRIVATE lib/libRobot/include)

#target_link_libraries(DOFBot PUBLIC imgui)
#target_link_libraries(DOFBot PUBLIC sfml-graphics sfml-audio)
target_link_libraries(DOFBot PUBLIC libarm)
#target_link_libraries(DOFBot PUBLIC imgui-sfml)
