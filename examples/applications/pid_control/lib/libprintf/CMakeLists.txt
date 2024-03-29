cmake_minimum_required(VERSION 3.23)

# Debug message
message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

# Register core library
add_library(libprintf INTERFACE)
target_sources(libprintf PUBLIC ${CMAKE_CURRENT_LIST_DIR}/printf/printf.c)
target_include_directories(libprintf INTERFACE ${CMAKE_CURRENT_LIST_DIR}/printf)

# Register other modules

# Debug message
message("Exiting ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")