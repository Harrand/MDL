cmake_minimum_required(VERSION 3.9)
project(mdl VERSION 0.6.0 DESCRIPTION "Comprehensive read/write library for the MDL data format")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
include_directories(mdl ${PROJECT_SOURCE_DIR}/src)
add_library(mdl SHARED
        src/mdl_file.cpp src/mdl_file.hpp
src/file.cpp src/file.hpp
src/xml_file.cpp src/xml_file.hpp)
target_link_libraries(mdl)