
********************************************************
TinyUSB
********************************************************

TinyUSB is a USB Host/Device library for embedded systems. In this article we will explore the library through the development of some very simple applications.

https://docs.tinyusb.org/en/latest/

Custom special purpose keyboard
===============================

It is very important to have a useful human interface to you applications. Unfortunately there is some drive towards limiting the use of custom hardware and instead relying on a generic keyboard (and mouse) for everything. A generic keyboard is good in the case that you have to use the same interface for many different purposes, but it is far from optimized for specific applications (except perhaps for text writing applications). This has long been a problem in industrial control applications, and other SCADA applications, but now it is even taking place in modern cars (e.g. in Teslas where you have to use a touch screen for even the most basic of tasks).

There are also some applications where even regular general purpose PC users can benefit from having a custom keyboard. E.g.:

* Software defined radios
* Software defined test and measurement equipment (such as a PC controlled oscilloscope)
* Music production software
* CAD software
* Computer gaming

In this example we will demonstrate how easily a custom keyboard can be implemented. It will have some encoders and push buttons to simplify the interface to applications with knobs and special purpose push buttons.


Hardware
--------

The STM32L476RG has built in hardware support for both USB host and device.


Microcontroller software
------------------------

PC software
-----------


USB to serial device
====================


USB microphone
==============


