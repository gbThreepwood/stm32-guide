
********************************************************
eRPC (Embedded RPC)
********************************************************

Embedded RPC (eRPC) is a remote procedure call library. The official repository is at: https://github.com/EmbeddedRPC/erpc


Remote control a LED from Python
================================

In this example we will remotely control a LED on the STM32L476RG from a Python application.

.. literalinclude:: ../../../examples/libraries/erpc/ledcontrol/idl/ledcontrol.erpc

Server program for the microcontroller
--------------------------------------

Generate the C++ version of the IDL (Interface description language) based code:

.. code-block:: console

    erpcgen -g c ledcontrol.erpc


UART DMA configuration
----------------------

UART DMA configuration is described in a separate page on this website. We will use the example from that page as a basis for our low level UART interface.

UART communication interface
----------------------------

We would not like to rely on any pre-made library UART communication driver, since that would mean that we have to give up any custom (e.g. ringbuffer DMA configuration) for UART communication. Instead we would like to add our own custom wrapper driver with callback functions which writes and reads the ringbuffers.

The eRPC library comes with support for the official CMSIS UART driver. We will write a custom implementation of this driver. The CMSIS driver has a lot of features, but our custom driver will only implement the bare minimum required to make it work with eRPC.

* Initialization of the UART is performed outside of our custom driver and thus the init function does not hold any low level UART initialization code. The init function does however register a callback to allow the UART driver to notify eRPC when a transfer (RX or TX) is complete. The driver needs to invoke this callback from ISR context, since eRPC uses a blocking wait for transfer to be complete.

Server program in C
-------------------

.. literalinclude:: ../../../examples/libraries/erpc/ledcontrol/main.c
   :language: c

Client program in Python
------------------------

The Python program will use a simple Tkinter GUI with a push button to toggle the LED. Feedback about the actual LED status will also be provided.


Generate the Python version of the IDL (Interface description language) based code:

.. code-block:: console

    erpcgen -g py ledcontrol.erpc


.. literalinclude:: ../../../examples/libraries/erpc/ledcontrol/python/ledcontrol.py

