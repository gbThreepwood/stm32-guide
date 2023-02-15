
*******************************************
STM32 Direct memory access (DMA) controller
*******************************************

The direct memory access controller allows us to move data between different parts of memory (RAM), or between peripherals and memory, without interaction with the CPU. This is useful for offloading the CPU in the cases that large amounts of data needs to be copied, or when a peripheral such as the ADC, or UART needs to store incoming data as it is converted or received.

The DMA request multiplexer (DMAMUX) is responsible for routing a given DMA request to a given DMA channel.

Memory to memory movement
=========================

In order to understand the DMA in the context of memory to memory transfers one must first know a little bit about how the memory in the STM32 microcontrollers operate.


In this example we will be configuring DMA1 for memory to memory transfer. Channel 3 is used, but any of the available channels could be used.


Enable the DMA clock:

.. code-block:: c

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

The DMA can be in either NORMAL, or CIRCULAR mode. When using memory to memory transfers only normal mode is supported.

.. code-block:: c

    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3, LL_DMA_DIRECTION_MEMORY_TO_MEMORY);
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_NORMAL);

If we want to transfer multiple data elements it is convenient if the DMA automatically increment both the source and the destination memory address. The LL function for configuring the source increment mode for memory to memory transfer is the same function which configures the peripheral increment mode.

.. code-block:: c

    // In memory to memory transfer peripheral increment is memory source increment
    // You should also try NOINCREMENT and observe the effect
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PERIPH_INCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MEMORY_INCREMENT);

The STM32 is 32-bit, hence a word is 32-bit wide. The memory addresses in the ARM architecture always refers to a byte (8 bit) of memory. The DMA can be configured to increment the addresses in steps of 1, 2, or 4 (i.e. 8, 16, or 32 bit) depending on the data we are copying.

.. code-block:: c

    // In memory to memory transfer peripheral size is memory source size
    // You should try to reduce the size from WORD to HALFWORD or BYTE and observe the effect
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PDATAALIGN_WORD);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MDATAALIGN_WORD);

Some of the microcontrollers in the STM32 series have multiple DMA controllers. Each controller has multiple channels, but will only operate on one channel at a time. For this reason it makes sense to be able to prioritize between the channels.

.. code-block:: c

    // The priority does not matter in this demo
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PRIORITY_VERYHIGH);

The data length register informs the DMA about the length (in bytes, half-words or words) of the data to be copied.

.. code-block:: c

    // Try to reduce the length and observe that the array is only partially copied
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, ARRAY_LEN(src_array_demo));

In order to easily calculate the length of the array the following macro is used:

.. code-block:: c

    #define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))

The DMA copies data from the memory address placed in the source register in to the memory address placed in the destination register. If we declare a variable for source and destination the :code:`&` operator provides us with the address. The value must be casted to :code:`uint32_t` in order to avoid compiler warnings.

.. code-block:: c

    LL_DMA_SetM2MSrcAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)&src_array_demo);
    LL_DMA_SetM2MDstAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)&dst_array_demo);


The DMA controller can generate a interrupt when it is half way through a transfer, and when the transfer is complete. A use case for the half transfer interrupt is to start dealing with the data while the other half of the buffer fills up. That way it is more easy to ensure that we do not miss any data.

.. code-block:: c

    // Enable half transfer (HT), and transfer complete (TC) interrupts
    //
    // Breakpoints (or watchpoints) inside the HT ISR will not easily allow us to observe that the DMA
    // is half way through the transfer, since the DMA will continue in the background
    // even when GDB breaks the main program.
    //
    // Additionally watchpoints are also no use since they only detect when the CPU tries
    // to change the variable, not when the DMA or some other peripheral is responsible for the change.
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_3);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);

When the DMA is in memory to memory mode, the transfer will begin as soon as the DMA channel is enabled.

.. code-block:: c

    // The transfer starts when the channel is enabled
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);


If you want to use interrupts, they must also be enabled in the nested vector interrupt controller (NVIC):

.. code-block:: c

    NVIC_SetPriority(DMA1_Channel3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);



Linking two GPIO registers
==========================

It is possible to use the DMA to map a variable on to a register controlling the GPIO, or to link the registers of two different GPIO ports by having the DMA copy the data from one register to the other.


Using a timer to trigger a DMA
==============================



Using DMA with peripherals
==========================

Using the DMA with a given peripheral is better described in the article covering the peripheral. Here we will only cover some general differences between this and the memory to memory movement.
