

#include <stm32l4xx_ll_bus.h>
#include <stm32l4xx_ll_rcc.h>
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_tim.h>

#include "bsp_timer.h"

void bsp_init_adc_timer(){

}

void bsp_init_interrupt_timer(){

}


void bsp_init_encoder_timer(){

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

    /* PA9 is on D8 */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);



    /*
     * TIM3 has CH1 on PA6 and CH2 on PA7
     * PA6 -> D12
     * PA7 -> D11
     *
     */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_6, LL_GPIO_PULL_DOWN);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_6, LL_GPIO_AF_2);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_7, LL_GPIO_PULL_DOWN);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_7, LL_GPIO_AF_2);

    //IS_TIM_ENCODER_INTERFACE_INSTANCE(TIM3);
    LL_TIM_SetEncoderMode(TIM3, LL_TIM_ENCODERMODE_X2_TI1);
    //LL_TIM_SetEncoderMode(TIM3, LL_TIM_ENCODERMODE_X4_TI12);

    LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);

    LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);

    LL_TIM_SetPrescaler(TIM3, 0);
    LL_TIM_SetCounterMode(TIM3, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(TIM3, 65535);
    LL_TIM_SetClockDivision(TIM3, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetRepetitionCounter(TIM3, 0);

    LL_TIM_DisableARRPreload(TIM3);
    LL_TIM_SetTriggerInput(TIM3, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM3);

    LL_TIM_SetCounter(TIM3, 0);
    LL_TIM_EnableCounter(TIM3);
}

