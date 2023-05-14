cmake_minimum_required(VERSION 3.23)

# Debug message
message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

# Register core library
add_library(fatfs INTERFACE)
target_sources(fatfs PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/fatfs/source/ff.c
    ${CMAKE_CURRENT_LIST_DIR}/fatfs/source/ffsystem.c
    ${CMAKE_CURRENT_LIST_DIR}/fatfs/source/ffunicode.c
    #${CMAKE_CURRENT_LIST_DIR}/fatfs/source/diskio.c
)
target_include_directories(fatfs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/fatfs/source)

# Register other modules

# Debug message
message("Exiting ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

