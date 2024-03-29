cmake_minimum_required(VERSION 3.23)

# Debug message
message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

# Register core library
add_library(libencoder INTERFACE)
target_sources(libencoder PUBLIC ${CMAKE_CURRENT_LIST_DIR}/libencoder.c)
target_include_directories(libencoder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/)

# Register other modules

# Debug message
message("Exiting ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")