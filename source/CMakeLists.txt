cmake_minimum_required(VERSION 3.2)

project(regex_project VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_library( regex_utilities INTERFACE stator.hpp )

add_executable( regex main.cpp )
#target_link_libraries(regex PRIVATE regex_utilities)
