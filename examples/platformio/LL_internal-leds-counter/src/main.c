/**
 * @file main.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief LED counter demo
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

void SysTick_Handler(void)
{

}

void init_gpio() {

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    LL_GPIO_SetPinMode(USER_LED_PORT, USER_LED1_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(USER_LED_PORT, USER_LED1_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(USER_LED_PORT, USER_LED2_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(USER_LED_PORT, USER_LED2_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(USER_LED_PORT, USER_LED3_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(USER_LED_PORT, USER_LED3_PIN, LL_GPIO_OUTPUT_PUSHPULL);

}

int main(void) {
    init_gpio();

    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    uint32_t port_value = 0;
    uint8_t counter = 0;
    while(1){

        port_value = (counter & 0x01) | (counter & 0x02)<<6 | (counter & 0x04)<<12;

        LL_GPIO_WriteOutputPort(USER_LED_PORT, port_value);
        LL_mDelay(500);

        counter += 1;
        if (counter > 7){
            counter = 0;
        }
    }

    return 0;
}
