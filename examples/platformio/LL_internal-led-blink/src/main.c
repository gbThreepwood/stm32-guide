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
#include <stm32f7xx_ll_gpio.h>
#include <stm32f7xx_ll_bus.h>
#include <stm32f7xx_ll_utils.h>

#define USER_LED_PORT           GPIOB
#define USER_LED1_PIN           LL_GPIO_PIN_0
#define USER_LED2_PIN           LL_GPIO_PIN_7
#define USER_LED3_PIN           LL_GPIO_PIN_14

void SysTick_Handler(void)
{

}

void init_gpio(void) {

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    LL_GPIO_SetPinMode(USER_LED_PORT, USER_LED1_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(USER_LED_PORT, USER_LED1_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}

int main(void) {
    init_gpio();

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    while(1){

        LL_GPIO_TogglePin(USER_LED_PORT, USER_LED1_PIN);
        LL_mDelay(500);
    }

    return 0;
}
