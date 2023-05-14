# STM32 Guide

Guide on how to develop for the STM32 series of microcontrollers, with emphasis on not using the official development tools.

## Getting started

The repository relies on some sub-modules, so after cloning you should also run:
`git submodule update --init --recursive`

## Motivation

The STM32CubeIDE is not that great in my opinion, altough the graphical pin configration tool is nice in some situations. Furthermore
the tool is specific for STM32, and it is not fun having to switch tool when swiching between differen microcontroller vendors.

In addition the official hardware abstraction layer (HAL) is too bloated and leaves to little control in the hand of the developer. There
are many guides on using CMSIS, but wery few on using the so called low level (LL) drivers from ST. The LL drivers provides a much
more readable code, without too much penalty on the performance.

The examples here uses CMake, and the official low level (LL) libraries from ST. Visual Studio Code has been used as development
environment, but any text editor will do.

The NUCLEO-L476RG has been selected for the examples. There is no particular reason for choosing this controller, except that I had it on hand.
It should be fairly easy to adapt the examples to different microcontrollers in the STM32 series, although the datasheet should be referenced
to check if a given pheripheral is available, and on which pin.

