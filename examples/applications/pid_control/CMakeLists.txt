cmake_minimum_required(VERSION 3.23)

set(STM32_TARGET_TRIPLET arm-none-eabi)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../../stm32-cmake/cmake/stm32_gcc.cmake)
set(STM32_CUBE_L4_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../STM32CubeL4)


project(pid_temp_control
        VERSION 0.0.1
        DESCRIPTION "PID temperature controler"
        HOMEPAGE_URL "https://github.com/gbThreepwood/"
        LANGUAGES C ASM
)

#set(QPC_CFG_PORT arm-cm)
#set(QPC_PROJECT "PID control")
#set(QPC_CFG_KERNEL qv)

#include(lib/qp/qpc/qpc_sdk_init.cmake)
set(QPC_PROJECT qpc_${PROJECT_NAME}) # the local project name within qpc
set(QPC_CFG_KERNEL ${CONFIG_KERNEL}) # the desired kernel (QV, QK or QXK)
set(QPC_CFG_QSPY ${CONFIG_QSPY}) # add QSPY support? (ON/OFF)
set(QPC_QFG_UNIT_TEST ${CONFIG_UNIT_TEST}) # add unit test support? (ON/OFF)
set(QPC_CFG_GUI ${CONFIG_GUI}) # is it a GUI application (e.g. win32 port) (ON/OFF)
set(QPC_CFG_VERBOSE ${CONFIG_VERBOSE})
set(QPC_CFG_PORT ${PORT})
#qpc_sdk_init()

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

find_package(CMSIS COMPONENTS STM32L4 REQUIRED)
find_package(HAL COMPONENTS STM32L4 LL_RCC LL_GPIO LL_UTILS LL_TIM REQUIRED)

add_definitions(-DUSE_FULL_LL_DRIVER)

set(MAIN_SOURCE_FILE main.c)

set(PROJECT_SOURCES
    src/main.c
    src/pid_control.c
    src/sys_ident.c
    src/active_objects/ledblink.c
    bsp/bsp_gpio.c
    bsp/bsp_adc.c
    bsp/bsp_dma.c
    bsp/bsp_timer.c
    bsp/bsp_uart.c
    bsp/interrupt_handlers.c
    bsp/qpc_callbacks.c
)

message("Source files: " ${PROJECT_SOURCES})

# STM32F4-Discovery
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE
    HAL::STM32::L4::LL_RCC
    HAL::STM32::L4::LL_GPIO
    HAL::STM32::L4::LL_UTILS
    HAL::STM32::L4::LL_TIM
    CMSIS::STM32::L476RG
    STM32::Nano
    STM32::NoSys
)

target_include_directories(${PROJECT_NAME} PRIVATE
    bsp/
    src/active_objects
)

# Add subdir with encoderlib and link to project
add_subdirectory("lib/encoder" libencoder)
target_link_libraries(${PROJECT_NAME} PRIVATE libencoder)

## Add subdir with hd44780lib and link to project
add_subdirectory("lib/hd44780" libhd44780)
target_link_libraries(${PROJECT_NAME} PRIVATE libhd44780)

# Add subdir with QP/C and link to project
add_subdirectory("lib/qp/" qpc)
target_link_libraries(${PROJECT_NAME} PRIVATE qpc)

# Add subdir with QP/C and link to project
#add_subdirectory("lib/qp/qpc" qpc)
#target_link_libraries(${PROJECT_NAME} PRIVATE qpc)

stm32_print_size_of_target(${PROJECT_NAME})


add_custom_target(flash DEPENDS ${PROJECT_NAME}.elf)
add_custom_command(TARGET flash
    USES_TERMINAL
    COMMAND "openocd" -f "interface/stlink.cfg" -c "transport select hla_swd" -c "adapter speed 300" -f "target/stm32l4x.cfg" -c "program ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf verify reset exit"
)

set(CMAKE_C_GDB ${GCC_PATH}/arm-none-eabi-gdb CACHE PATH "" FORCE)

add_custom_target(gdb DEPENDS ${PROJECT_NAME}.elf)
add_custom_command(TARGET gdb
    COMMAND openocd -f interface/stlink.cfg -f target/stm32wlx.cfg &
    COMMAND arm-none-eabi-gdb -q ${PROJECT_NAME}.elf -ex "target extended-remote localhost:3333"
)

#set(CMAKE_C_GDB ${GCC_PATH}/arm-none-eabi-gdb CACHE PATH "" FORCE)

add_custom_target(openocd DEPENDS ${PROJECT_NAME}.elf)
add_custom_command(TARGET openocd
    COMMAND openocd -f interface/stlink.cfg -f target/stm32l4x.cfg
    #COMMAND arm-none-eabi-gdb -q ${PROJECT_NAME}.elf -ex "target extended-remote localhost:3333"
)
