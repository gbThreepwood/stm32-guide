
*******************************************
STM32 Analog to digital converter (ADC)
*******************************************

.. warning:: This article is a work in progress

The ADC supports 12 external channels, as well as 4 internal channels. The internal channels are connected to a temperature sensor, a reference voltage, one of the DAC outputs, and the battery supply pin of the microcontroller.



Configure the ADC for polling
=============================


Self calibration
----------------

The ADC has a build in calibration feature where it calculates a error correction value for each of the internal sampling capacitors. It is recommended to run this calibration on start up of the microcontroller, before the ADC is used.


Sequencer configuration
-----------------------

The ADC in the STM32 uses a configurable sequencer to automatically step through the various input channels in order to continuously convert multiple inputs. Even if you are manually triggering the conversion from software, the sequencer must be configured for the channels of interest. Each time you initiate a conversion the ADC peripheral will select the next input in the sequence, wrapping around as it finished with the last input.

The channels in the sequencer can be placed in any arbitrary order.

If you insist on the "Arduino way" of doing things (i.e. a call to some function such as: :code:`adc_read(channel_1)` should initiate a conversion on the specified channel and return the converted value), you will have to implement some library around the low level API. Such libraries are available, but if you think that you want to do it this way you are (probably) wrong.

.. code-block:: c

    LL_ADC_REG_SetSequencerRanks(ADC, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_4);
    LL_ADC_REG_SetSequencerRanks(ADC, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_5);



.. code-block:: c

    LL_ADC_REG_ReadConversionData12(ADC);




Temperature, battery, and reference voltage channel
---------------------------------------------------

.. code-block:: c

    LL_ADC_REG_SetSequencerRanks(ADC, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_TEMPSENSOR);
    LL_ADC_SetChannelSamplingTime(ADC, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SAMPLINGTIME_39CYCLES_5);
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC), LL_ADC_PATH_INTERNAL_TEMPSENSOR | LL_ADC_PATH_INTERNAL_VREFINT | LL_ADC_PATH_INTERNAL_VBAT); 

    LL_ADC_REG_SetSequencerRanks(ADC, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_VREFINT);
    LL_ADC_SetChannelSamplingTime(ADC, LL_ADC_CHANNEL_VREFINT, LL_ADC_SAMPLINGTIME_39CYCLES_5);

    LL_ADC_REG_SetSequencerRanks(ADC, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_VBAT);
    LL_ADC_SetChannelSamplingTime(ADC, LL_ADC_CHANNEL_VBAT, LL_ADC_SAMPLINGTIME_39CYCLES_5);


Continuos conversion and interrupt
==================================

If continuous conversions are needed it is possible to use a hardware timer to initiate them. A interrupt on the timer can then be used to activate the ADC. Alternatively a software timer, or time event generator can also be used.

The ADC is capable of generating an interrupt on end of conversion. This interrupt can be used to signal the main application that new data is available, e.g. by publishing a "ADC_EVENT" signal.

Continuos conversion and DMA
============================

The DMA is the most efficient way to transfer converted data to memory. It is the ideal solution if high conversion rates are needed, but it might also be used with lower conversion rates.

The DMA controller has multiple channels, and could be utilized for several different peripherals in a single application. If this is the case it becomes important to verify that there are no conflicts in the DMA setup. Make sure that you perform thorough testing of all peripherals after making adjustments in the configuration of a single peripheral. In practice this should be performed automatically by means of unit and integration testing.

