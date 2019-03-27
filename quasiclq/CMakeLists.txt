cmake_minimum_required (VERSION 3.4)

project (quasiclq)

aux_source_directory(. SRC_LIST)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(quasiclq ${SRC_LIST})