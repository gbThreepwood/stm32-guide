
*******************************************
STM32 Digital to analog converter (DAC)
*******************************************

The STM32L476RG comes with two 12-bit digital to analog converters. The outputs are not based on PWM, but rater on a integrated resistor string connected to an inverting operational amplifier.


Generating a sine wave
======================

.. literalinclude:: ../../../examples/dac/basic_dac/basic_dac.c
   :language: c

.. literalinclude:: ../../../examples/dac/basic_dac/sine_table.h
   :language: c



Using the DAC with DMA
======================

The DAC supports receiving the digital output value from the DMA. In this example we will use the DMA to transfer data from a buffer to the DAC. The buffer will hold some PCM data extracted from a WAV file so we can demonstrate playing of real music. The next step would be to read the WAV file from a SD-card.




The WAV file format
-------------------

https://docs.fileformat.com/audio/wav/

