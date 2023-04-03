.. STM32 programming by examples documentation master file, created by
   sphinx-quickstart on Wed Feb 15 20:44:36 2023.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

STM32 programming by examples
=========================================================

This book and example repository is still very much a work in progress. Some chapters are empty, and simply added to function as a to do list.

The complete source code for the examples is available at: https://github.com/gbThreepwood/stm32-guide/tree/master/examples

.. toctree::
   :maxdepth: 2
   :caption: Peripherals:

   Introduction <introduction.rst>
   STM32 GPIO <peripherals/stm32-gpio.rst>
   STM32 Timer <peripherals/stm32-timer.rst>
   STM32 DMA <peripherals/stm32-dma.rst>
   STM32 ADC <peripherals/stm32-adc.rst>
   STM32 UART <peripherals/stm32-uart.rst>
   STM32 RTC <peripherals/stm32-rtc.rst>
   STM32 Low power <peripherals/stm32-low-power.rst>
   STM32 I2C <peripherals/stm32-i2c.rst>
   STM32 SPI <peripherals/stm32-spi.rst>

   STM32 Advanced timer <peripherals/stm32-advanced-timer.rst>

.. toctree::
   :maxdepth: 2
   :caption: Applications

   SinePWM <motor-control/sine-pwm.rst>
   SV-PWM <motor-control/sv-pwm.rst>
   Field oriented control <motor-control/field-oriented-control.rst>
   Direct torque control <motor-control/direct-torque-control.rst>

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
