/**
 * @file uart_print.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief UART demo
 * @version 0.1
 * @date 2023-01-30
 * 
 * @copyright Copyright (c) 2023
 * 
 * Transmit data on one of the UARTs on the Nucleo-L476RG
 * 
 */
#include <stm32l4xx_ll_gpio.h>
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
    //LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    //LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

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

int main(void) {
    init_gpio();

    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    init_uart();

    while(1) {
        while(!LL_USART_IsActiveFlag_TC(USART2));

        LL_USART_TransmitData8(USART2, '&');
    };

    return 0;
}
