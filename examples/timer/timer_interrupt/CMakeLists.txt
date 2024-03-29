cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../../stm32-cmake/cmake/stm32_gcc.cmake)
set(STM32_CUBE_L4_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../STM32CubeL4)

project(timer_interrupt C ASM)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

find_package(CMSIS COMPONENTS STM32L4 REQUIRED)
find_package(HAL COMPONENTS STM32L4 LL_RCC LL_GPIO LL_UTILS LL_PWR LL_TIM REQUIRED)

add_definitions(-DUSE_FULL_LL_DRIVER)

# STM32F4-Discovery
add_executable(${PROJECT_NAME} timer_interrupt.c)
target_link_libraries(${PROJECT_NAME}
    HAL::STM32::L4::LL_RCC
    HAL::STM32::L4::LL_GPIO
    HAL::STM32::L4::LL_UTILS
    HAL::STM32::L4::LL_PWR
    HAL::STM32::L4::LL_TIM
    CMSIS::STM32::L476RG
    STM32::NoSys
)
stm32_print_size_of_target(${PROJECT_NAME})


add_custom_target(flash DEPENDS ${PROJECT_NAME}.elf)
add_custom_command(TARGET flash
    USES_TERMINAL
    COMMAND "openocd" -f "interface/stlink.cfg" -c "transport select hla_swd" -c "adapter speed 300" -f "target/stm32l4x.cfg" -c "program ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf verify reset exit"
)

#set(CMAKE_C_GDB ${GCC_PATH}/arm-none-eabi-gdb CACHE PATH "" FORCE)

add_custom_target(openocd DEPENDS ${PROJECT_NAME}.elf)
add_custom_command(TARGET openocd
    COMMAND openocd -f interface/stlink.cfg -f target/stm32l4x.cfg
    #COMMAND arm-none-eabi-gdb -q ${PROJECT_NAME}.elf -ex "target extended-remote localhost:3333"
)