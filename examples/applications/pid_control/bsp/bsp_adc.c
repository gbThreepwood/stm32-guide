

#include <stm32l4xx_ll_bus.h>
#include <stm32l4xx_ll_rcc.h>
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_adc.h>

#include "bsp_adc.h"

void calibrate_adc();


/**
 * @brief Init ADC
 *
 */
void bsp_init_adc() {

    // Pin A0 is connected to PA0 which supports ADC1_IN5

    // ADC conversions can be organized in to two groups, regular and injected
    // The regular conversion runs continously, while the injected conversion
    // must be triggered by software, or some external event.
    // Injected conversion has higher priority than regular and
    // is useful for e.g. motor control, where it allows
    // us to start conversion of the current measurements
    // on specific favorable time instants of the
    // transistor switching cycle.

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

    // A0 pin is used for the TMP36 temperature sensor
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
    LL_GPIO_EnablePinAnalogControl(GPIOA, LL_GPIO_PIN_0);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);

    // A1 pin is used for a potmeter for manual adjustments (multi purpose)
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);
    LL_GPIO_EnablePinAnalogControl(GPIOA, LL_GPIO_PIN_1);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_1, LL_GPIO_PULL_NO);

    NVIC_SetPriority(ADC1_2_IRQn, 0);
    NVIC_EnableIRQ(ADC1_2_IRQn);

    /**
     * Configure the ADC clock
     *
     */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);

    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV4);

    LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_SYSCLK);


    //LL_ADC_Disable(ADC1);
    while(__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE()); // Block here if ADC is enabled (it should not be)


    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);
    LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);
    LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_MODE_NONE);

    //LL_ADC_AWD_CH_VREFINT_REG

    //LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};
    //LL_ADC_InitTypeDef ADC_InitStruct = {0};
    //LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};

    /**
     * @brief
     *
     * Scan mode can only be used with DMA.
     */
    // Sequencer disabled is equivalent to sequencer of 1 rank, ADC conversion on only 1 channel.
    //LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS);
    LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);

    /**
     * @brief
     *
     * The sequencer discont mode can not be used with continous mode
     */
    LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_1RANK);

    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);
    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);
    LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

    LL_ADC_DisableIT_EOC(ADC1);
    LL_ADC_DisableIT_EOS(ADC1);

    LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
    LL_ADC_SetOverSamplingDiscont(ADC1, LL_ADC_OVS_REG_DISCONT);

    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_TEMPSENSOR | LL_ADC_PATH_INTERNAL_VREFINT | LL_ADC_PATH_INTERNAL_VBAT);

    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_5);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_6);

    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_3 , LL_ADC_CHANNEL_TEMPSENSOR);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_4 , LL_ADC_CHANNEL_VBAT);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_5 , LL_ADC_CHANNEL_VREFINT);

    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_5, LL_ADC_SAMPLINGTIME_92CYCLES_5);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_6, LL_ADC_SAMPLINGTIME_47CYCLES_5);

    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SAMPLINGTIME_24CYCLES_5);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_VBAT, LL_ADC_SAMPLINGTIME_24CYCLES_5);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_VREFINT, LL_ADC_SAMPLINGTIME_24CYCLES_5);

    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_5, LL_ADC_SINGLE_ENDED);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_6, LL_ADC_SINGLE_ENDED);

    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SINGLE_ENDED);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_VBAT, LL_ADC_SINGLE_ENDED);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_VREFINT, LL_ADC_SINGLE_ENDED);

    /**
     * @brief Software procedure to enable the ADC
     *
     * From the reference manual for STM32l4, page 519.
     *
     */
    LL_ADC_DisableDeepPowerDown(ADC1);

    LL_ADC_EnableInternalRegulator(ADC1);

    calibrate_adc();

    LL_ADC_Enable(ADC1);
    while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0){}

}

/**
 * @brief Calibrate ADC in order to improve accuracy
 *
 */
void calibrate_adc() {

    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0);
}

