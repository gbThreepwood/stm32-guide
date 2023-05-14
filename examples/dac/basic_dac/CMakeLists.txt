cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../../stm32-cmake/cmake/stm32_gcc.cmake)
set(STM32_CUBE_L4_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../STM32CubeL4)

project(basic_dac C ASM)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

find_package(CMSIS COMPONENTS STM32L4 REQUIRED)
find_package(HAL COMPONENTS STM32L4 LL_RCC LL_GPIO LL_DAC LL_UTILS REQUIRED)

# STM32F4-Discovery
add_executable(${PROJECT_NAME} basic_dac.c)
target_link_libraries(${PROJECT_NAME}
    HAL::STM32::L4::LL_RCC
    HAL::STM32::L4::LL_GPIO
    HAL::STM32::L4::LL_DAC
    HAL::STM32::L4::LL_UTILS
    CMSIS::STM32::L476RG
    STM32::NoSys 
)

#target_include_directories()
stm32_print_size_of_target(${PROJECT_NAME})

add_definitions(-DUSE_FULL_LL_DRIVER)

add_custom_target(flash DEPENDS ${PROJECT_NAME}.elf)
add_custom_command(TARGET flash
    USES_TERMINAL
    COMMAND "openocd" -f "interface/stlink.cfg" -c "transport select hla_swd" -c "adapter speed 300" -f "target/stm32l4x.cfg" -c "program ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf verify reset exit"
)
