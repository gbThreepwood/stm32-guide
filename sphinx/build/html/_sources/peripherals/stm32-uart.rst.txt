
********************************************************
STM32 Universal asynchronous receiver transmitter (UART)
********************************************************

There is very little to learn from blindly following a guide to achieve some goal, and this page does not attempt to provide every little detail which is required in order for UART to work in your project. The reader should understand each step, in order to be able to read between the lines the extra steps which are required. By "extra steps" is meant such things as the proper declaration of variables and functions, the inclusion of initialization code generated from CubeMX, etc. An understanding of the C language is thus also required.

Using the HAL for RX and TX
===========================

A quick google search will reveal a lot of STM32 UART examples which are not amazing. They might work for the simple demonstration that they provide, but they are not easy to extend to something useful. Some examples are good, but the reader needs to be careful. Especially the HAL library is not too flexible.

The conclusion is that it is probably better to stick with the low level (LL) drivers, and possibly add some application specific high level abstractions on top of them. Still the basics of the UART HAL drivers should be understood in case one encounters them in some applications or example code.

The :code:`printf()` family of functions is nice if you want to manipulate strings. A quick way to integrate it with the HAL UART drivers is as follows:

.. code-block:: c
    
    char uart_tx_buf[32];

    sprintf(uart_tx_buf, "Test\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)uart_tx_buf, strlen(uart_tx_buf), 100);


Receive using polling
---------------------

.. warning:: This section needs to be written


Receive using interrupts
------------------------

The :code:`HAL_UART_Receive_IT(&huart1, &uart_rx_buf, number_of_bytes);` configures a interrupt to receive a specified number of bytes. This function is not particularly useful as we would normally want to receive whatever the UART sees, not a specific number of bytes. It is possible to set the number of bytes to one, and call the function each time a byte is received, but the author has found the reliability of this approach to be less than adequate. It is better to simply enable receive interrupts, and handle the received data in an ISR:

.. code-block:: c

    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);

The ISR for USART1 can then simply use the non-interrupt version of the UART receive functions :code:`HAL_UART_Receive();` to acquire the received byte:

.. code-block:: c

    uint8_t uart_rx_char = 0x00;

    void USART1_IRQHandler(void)
    {
        HAL_UART_Receive(&huart1, &uart_rx_char, 1, 1);

        //HAL_UART_IRQHandler(&huart1); // We do not need to call the HAL interrupt handler
    }

Low level access to the UART receive buffer through direct register read or the low level (LL) API will probably be more efficient, but here we are discussing the HAL drivers.

In the above example a single byte buffer is used, but typically one would implement a ring (FIFO) buffer where the ISR places each received byte. If the buffer is full it could either drop the latest received data, or overwrite the oldest data in the ring.


Using the low level (LL) drivers
================================







Circular buffer for UART RX and TX with interrupts
==================================================


https://github.com/atakansarioglu/c-circus
https://github.com/AndersKaloer/Ring-Buffer













