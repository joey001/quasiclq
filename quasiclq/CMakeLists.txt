cmake_minimum_required (VERSION 3.4)

project (quasiclq)

aux_source_directory(. SRC_LIST)
if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-std=c++11)
    message(STATUS "optional:-std=c++11")   
endif(CMAKE_COMPILER_IS_GNUCXX)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(quasiclq ${SRC_LIST})
