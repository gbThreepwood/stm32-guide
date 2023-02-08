/**
 * @file ledblink.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief LED blink demo
 * @version 0.1
 * @date 2023-01-30
 * 
 * @copyright Copyright (c) 2023
 * 
 * Blinks external LEDs connected to pin D8, D9 and D10 on the Nucleo-L476RG
 * 
 */
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_cortex.h>
#include <stm32l4xx_ll_rcc.h>
#include "stm32l4xx_ll_bus.h"

#define BLUE_LED_PORT          GPIOA
#define BLUE_LED_PIN           LL_GPIO_PIN_9

#define GREEN_LED_PORT         GPIOC
#define GREEN_LED_PIN          LL_GPIO_PIN_7

#define RED_LED_PORT           GPIOB
#define RED_LED_PIN            LL_GPIO_PIN_6

void SysTick_Handler(void)
{
    static uint_fast32_t counter = 0;
    counter++;

    // 1 Hz blinking
    if ((counter % 500) == 0)
        LL_GPIO_TogglePin(BLUE_LED_PORT, BLUE_LED_PIN);

    // 2 Hz blinking
    if ((counter % 250) == 0)
        LL_GPIO_TogglePin(GREEN_LED_PORT, GREEN_LED_PIN);

    // 4 Hz blinking
    if ((counter % 125) == 0)
        LL_GPIO_TogglePin(RED_LED_PORT, RED_LED_PIN);

}

void init_gpio() {

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    LL_GPIO_SetPinMode(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GREEN_LED_PORT, GREEN_LED_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(RED_LED_PORT, RED_LED_PIN, LL_GPIO_MODE_OUTPUT);

    LL_GPIO_SetPinOutputType(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(GREEN_LED_PORT, GREEN_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(RED_LED_PORT, RED_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}

int main(void) {
    init_gpio();

    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    while(1);

    return 0;
}
