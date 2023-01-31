# STM32 Guide

Guide on how to develop for the STM32 series of microcontrollers, with emphasis on not using the official development tools.

The STM32CubeIDE is not that great in my opinoin, altough the graphical pin configration tool is nice in some situations. Furthermore
the tool is specific for STM32, and it is not fun having to switch tool when swiching between differen microcontroller vendors.

In addition the official hardware abstraction layer (HAL) is too bloated and leaves to little control in the hand of the developer. There
are many guides on using CMSIS, but wery few on using the so called low level (LL) drivers from ST. The LL drivers provides a much
more readable code, without too much penalty on the performance.

The examples here uses CMake, and the official low level (LL) libraries from ST. Visual Studio Code has been used as development
environment, but any text editor will do.

