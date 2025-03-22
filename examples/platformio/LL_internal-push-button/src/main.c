/**
 * @file main.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief Internal push button demo
 * @version 0.1
 * @date 2025-03-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <stm32f7xx_ll_gpio.h>
#include <stm32f7xx_ll_cortex.h>
#include <stm32f7xx_ll_rcc.h>
#include <stm32f7xx_ll_bus.h>
#include <stm32f7xx_ll_utils.h>

#define USER_LED_PORT           GPIOB
#define USER_LED1_PIN           LL_GPIO_PIN_0
#define USER_LED2_PIN           LL_GPIO_PIN_7
#define USER_LED3_PIN           LL_GPIO_PIN_14

#define USER_BTN_PORT           GPIOC
#define USER_BTN_PIN            LL_GPIO_PIN_13

void SysTick_Handler(void)
{

}

void init_gpio() {

    // Aktiver klokker
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

    // Konfigurer utgangar
    LL_GPIO_SetPinMode(USER_LED_PORT, USER_LED1_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(USER_LED_PORT, USER_LED1_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(USER_LED_PORT, USER_LED2_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(USER_LED_PORT, USER_LED2_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(USER_LED_PORT, USER_LED3_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(USER_LED_PORT, USER_LED3_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    // Konfigurer inngangar
    LL_GPIO_SetPinMode(USER_BTN_PORT, USER_BTN_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(USER_BTN_PORT, USER_BTN_PIN, LL_GPIO_PULL_NO); // Inngangen har ekstern "pull down"
}

int main(void) {
    init_gpio();

    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    while(1){

        if(LL_GPIO_IsInputPinSet(USER_BTN_PORT, USER_BTN_PIN)){
            LL_mDelay(100);
        }
        else{
            LL_mDelay(500);
        }

        LL_GPIO_TogglePin(USER_LED_PORT, USER_LED1_PIN);

    }

    return 0;
}
