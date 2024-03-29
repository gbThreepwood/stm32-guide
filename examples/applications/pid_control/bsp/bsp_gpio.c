

#include <stm32l4xx_ll_bus.h>
#include <stm32l4xx_ll_gpio.h>

#include "bsp_gpio.h"



/**
 * @brief Initialize GPIO
 *
 */
void bsp_init_gpio() {

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(ONBOARD_LED_PORT, ONBOARD_LED_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(ONBOARD_LED_PORT, ONBOARD_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}

