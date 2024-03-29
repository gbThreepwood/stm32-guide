cmake_minimum_required(VERSION 3.23)

# Debug message
message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

# Register core library
add_library(libhd44780 INTERFACE)
target_sources(libhd44780 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/hd44780.c)
target_include_directories(libhd44780 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/)

# Register other modules

# Debug message
message("Exiting ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")