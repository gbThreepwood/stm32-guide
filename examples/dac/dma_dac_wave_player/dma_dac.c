/**
 * @file dma_dac.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief DMA DAC wave player
 * @version 0.1
 * @date 2023-01-30
 * 
 * @copyright Copyright (c) 2023
 * 
 * Read a wave file from a SD-card and transfer the PCM encoded audio using DMA to the DAC of the Nucleo-L476RG
 * 
 */
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_cortex.h>
#include <stm32l4xx_ll_rcc.h>
#include <stm32l4xx_ll_bus.h>
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_pwr.h"
#include <stm32l4xx_ll_dac.h>
#include <stm32l4xx_ll_utils.h>


#define BLUE_LED_PORT          GPIOA
#define BLUE_LED_PIN           LL_GPIO_PIN_9

#define VOLTAGE_REFERENCE      3.3
#define SINE_WAVE
//#define SAWTOOTH_WAVE

void SysTick_Handler(void)
{
    static uint_fast32_t counter = 0;
    counter++;

    // 1 Hz blinking
    if ((counter % 500) == 0)
        LL_GPIO_TogglePin(BLUE_LED_PORT, BLUE_LED_PIN);

}

void init_clock(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
    while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_4)
    {
    }

    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    LL_RCC_HSI_Enable();    
     /* Wait till HSI is ready */
    while(LL_RCC_HSI_IsReady() != 1)
    {   
    }

    LL_RCC_HSI_SetCalibTrimming(16);
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 10, LL_RCC_PLLR_DIV_2);
    LL_RCC_PLL_EnableDomain_SYS();
    LL_RCC_PLL_Enable();

     /* Wait till PLL is ready */
    while(LL_RCC_PLL_IsReady() != 1)
    {   
    }

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

     /* Wait till System clock is ready */
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {   
    }

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    LL_Init1msTick(80000000);   
    LL_SetSystemCoreClock(80000000);
}

void init_gpio() {

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    //LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    //LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    LL_GPIO_SetPinMode(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_MODE_OUTPUT);

    LL_GPIO_SetPinOutputType(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}

void init_dac(){

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);

    /**
     * @brief 
     * 
     * DAC output 1 is on PA4. On the Nucleo board this is connector A2.
     * DAC output 2 is on PA5. On the Nucleo board this is CN5 pin 6 and CN10 pin 11 if solder bridge SB42 is closed (it is closed by default).
     * DAC output 2 is on PA5. On the Nucleo board this is CN10 pin 30 if solder bridge SB44 is closed (it is open by default).
     */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_ANALOG);

    LL_DAC_InitTypeDef DAC_init_struct = {0};

    // Enabling the output buffer causes some voltage offset, and clipping of the maximum and minimum values.
    DAC_init_struct.OutputBuffer = LL_DAC_OUTPUT_BUFFER_DISABLE;

    DAC_init_struct.OutputConnection = LL_DAC_OUTPUT_CONNECT_GPIO;
    DAC_init_struct.OutputMode = LL_DAC_OUTPUT_MODE_NORMAL;
    DAC_init_struct.TriggerSource = LL_DAC_TRIG_SOFTWARE;
    DAC_init_struct.WaveAutoGeneration = LL_DAC_WAVE_AUTO_GENERATION_NONE;

    DAC_init_struct.WaveAutoGenerationConfig = LL_DAC_TRIANGLE_AMPLITUDE_1;

    LL_DAC_Init(DAC1, LL_DAC_CHANNEL_1, &DAC_init_struct);

    LL_DAC_SetMode(DAC1, LL_DAC_CHANNEL_1, LL_DAC_MODE_NORMAL_OPERATION);

    LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);
}


void write_dac_raw(uint16_t value_raw){

    LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, value_raw);
    LL_DAC_TrigSWConversion(DAC1, LL_DAC_CHANNEL_1);
}


int main(void) {

    init_clock(); // Increase the clock speed in order to increase the output waveform frequency

    init_gpio();

    init_dac();


    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    for(;;){

    }

    return 0;
}

