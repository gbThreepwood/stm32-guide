/**
 * @file adc_read.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief ADC demo
 * @version 0.1
 * @date 2023-01-30
 * 
 * @copyright Copyright (c) 2023
 * 
 * Reads some analog inputs on the Nucleo-L476RG
 * 
 */
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_adc.h>
#include <stm32l4xx_ll_usart.h>

#include <stm32l4xx_ll_cortex.h>
#include <stm32l4xx_ll_rcc.h>
#include "stm32l4xx_ll_bus.h"

#define BLUE_LED_PORT          GPIOA
#define BLUE_LED_PIN           LL_GPIO_PIN_9

#define UART2_PORT             GPIOA
#define UART2_TX_PIN           LL_GPIO_PIN_2
#define UART2_RX_PIN           LL_GPIO_PIN_3

void SysTick_Handler(void)
{
    static uint_fast32_t counter = 0;
    counter++;

    // 1 Hz blinking
    if ((counter % 500) == 0)
        LL_GPIO_TogglePin(BLUE_LED_PORT, BLUE_LED_PIN);

}

void init_gpio() {

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}

void init_uart() {
    // From the Nucleo manual:
    // By default, the USART2 communication between the target STM32 and ST-LINK MCU is enabled.
    // PA2 for TX and PA3 for RX

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode(UART2_PORT, UART2_TX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(UART2_PORT, UART2_RX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(UART2_PORT, UART2_TX_PIN, LL_GPIO_AF_7);
    LL_GPIO_SetAFPin_0_7(UART2_PORT, UART2_RX_PIN, LL_GPIO_AF_7);


    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);

    LL_USART_SetOverSampling(USART2, LL_USART_OVERSAMPLING_8);

    LL_USART_SetBaudRate(USART2, SystemCoreClock, LL_USART_OVERSAMPLING_8, 9600);

    LL_USART_EnableDirectionRx(USART2);
    LL_USART_EnableDirectionTx(USART2);

    LL_USART_Enable(USART2);
}

void init_adc() {

    // Pin A0 is connected to PA0 which supports ADC1_IN5
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);

    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV4);

    LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);

    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_5, LL_ADC_SAMPLINGTIME_24CYCLES_5);

    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_5, LL_ADC_SINGLE_ENDED);

    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_5);

    //LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2 , LL_ADC_CHANNEL_TEMPSENSOR);
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_CONTINUOUS);

    LL_ADC_Enable(ADC1);
    LL_ADC_REG_StartConversion(ADC1);
}

// Convert int to string representation in decimal number system
// The caller should provide a pointer to the end of a buffer with
// size of at least 3*sizeof(uint32_t) + 1
char *intToStrDec(uint32_t x, char *s)
{
    *--s = 0; // Decrement pointer and NULL terminate string
    if (!x) *--s = '0'; // Add a zero in ASCII if x is zero
    for (; x; x/=10) *--s = '0'+x%10; // Loop as long as x is not equal to zero, divide by 10 and add the correct digit
    return s; // Return a pointer to the beginning of the string (this does not have to be the beginning of the buffer)
}

void print_uint32(uint32_t val) {

    char buf[3*sizeof(uint32_t)+1];

    char *str = intToStrDec(val, buf + sizeof(buf));

    while(!LL_USART_IsActiveFlag_TC(USART2));

    LL_USART_TransmitData8(USART2, '&');
}

int main(void) {
    init_gpio();
    init_adc();

    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    while(1) {

        while(!LL_ADC_IsActiveFlag_EOC(ADC1));
        
        uint32_t data = LL_ADC_REG_ReadConversionData32(ADC1);
        
    }

    return 0;
}
