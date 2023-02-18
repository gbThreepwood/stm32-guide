
***********
STM32 guide
***********

For each chapter in this book there exists one or more example project which demonstrate the topics covered in the chapter.

The STM32L476RG is used in all the examples, but it should be relatively straight forward to adapt the code to a different microcontroller in the STM32 series.

The book is not able to cover every aspect of every microcontroller in the STM32 series. In the hope that it will provide the reader with a more generic skill set, some emphasis will be put on teaching the reader to find the relevant information in the datasheet and reference manual from ST.

Running the examples
====================

The examples uses CMake as the build system.

Create a new directory named :code:`build` inside the given example root directory. Run the following commands in the build directory:

.. code-block:: console

    cmake ..
    make
    make flash

Alternatively you may copy the example code in to a project of your own. E.g. a project created in PlatformIO or (god forbid) in STM32CubeIDE.

Debugging
=========

Even in the unlikely case that the examples are bug free, the debugger is still a very useful tool to learn how the microcontroller behaves in different situations.

