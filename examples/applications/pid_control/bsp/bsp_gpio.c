
#include <stdbool.h>

#include <stm32l4xx_ll_bus.h>
#include <stm32l4xx_ll_gpio.h>

#include "bsp_gpio.h"

#define HD44780_PORT GPIOC
#define HD44780_D4_PIN LL_GPIO_PIN_13
#define HD44780_D5_PIN LL_GPIO_PIN_12
#define HD44780_D6_PIN LL_GPIO_PIN_11
#define HD44780_D7_PIN LL_GPIO_PIN_10

#define HD44780_EN_PIN LL_GPIO_PIN_2
//#define DH44780_RW_PIN LL_GPIO_PIN10
#define HD44780_RS_PIN LL_GPIO_PIN_3

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

    /* HD44780 LCD connections */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    LL_GPIO_SetPinMode(HD44780_PORT, HD44780_D4_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(HD44780_PORT, HD44780_D4_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(HD44780_PORT, HD44780_D5_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(HD44780_PORT, HD44780_D5_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(HD44780_PORT, HD44780_D6_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(HD44780_PORT, HD44780_D6_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(HD44780_PORT, HD44780_D7_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(HD44780_PORT, HD44780_D7_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(HD44780_PORT, HD44780_EN_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(HD44780_PORT, HD44780_EN_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(HD44780_PORT, HD44780_RS_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(HD44780_PORT, HD44780_RS_PIN, LL_GPIO_OUTPUT_PUSHPULL);

}


void bsp_lcd_set_gpio_pins(uint8_t data, bool rs, bool rw){

    rs ? LL_GPIO_SetOutputPin(HD44780_PORT, HD44780_RS_PIN) : LL_GPIO_ResetOutputPin(HD44780_PORT, HD44780_RS_PIN);
    ((data >> 0) & 0x01) ? LL_GPIO_SetOutputPin(HD44780_PORT, HD44780_D4_PIN) : LL_GPIO_ResetOutputPin(HD44780_PORT, HD44780_D4_PIN);
    ((data >> 1) & 0x01) ? LL_GPIO_SetOutputPin(HD44780_PORT, HD44780_D5_PIN) : LL_GPIO_ResetOutputPin(HD44780_PORT, HD44780_D5_PIN);
    ((data >> 2) & 0x01) ? LL_GPIO_SetOutputPin(HD44780_PORT, HD44780_D6_PIN) : LL_GPIO_ResetOutputPin(HD44780_PORT, HD44780_D6_PIN);
    ((data >> 3) & 0x01) ? LL_GPIO_SetOutputPin(HD44780_PORT, HD44780_D7_PIN) : LL_GPIO_ResetOutputPin(HD44780_PORT, HD44780_D7_PIN);

    //if(rw)
    //    LL_GPIO_SetOutputPin(HD44780_PORT, HD44780_RW_PIN);
    //else
    //    LL_GPIO_SetOutputPin(HD44780_PORT, HD44780_RW_PIN);

    //uint32_t port_data = LL_GPIO_ReadOutputPort(HD44780_PORT);

    /* The four LSB of data should go to D7 to D4
     * There are functions for set and clear specific pins, but not functions which
     * takes the value to write to a given pin #why?
     * Solution is to read the whole port, and then modify our bits, and then write
     * back. Hopefully this does not cause any concurrency issues.
     */
    //port_data &= ~(1 << HD44780_D4_PIN) | (((data >> 0) & 0x01) << HD44780_D4_PIN);
    //port_data &= ~(1 << HD44780_D5_PIN) | (((data >> 1) & 0x01) << HD44780_D5_PIN);
    //port_data &= ~(1 << HD44780_D6_PIN) | (((data >> 2) & 0x01) << HD44780_D6_PIN);
    //port_data &= ~(1 << HD44780_D6_PIN) | (((data >> 3) & 0x01) << HD44780_D7_PIN);

    //LL_GPIO_WriteOutputPort(HD44780_PORT, port_data);

}

void bsp_lcd_set_en_pin(bool en){
    en ? LL_GPIO_SetOutputPin(HD44780_PORT, HD44780_EN_PIN) : LL_GPIO_ResetOutputPin(HD44780_PORT, HD44780_EN_PIN);
}

void bsp_green_led_toggle(){

    LL_GPIO_TogglePin(ONBOARD_LED_PORT, ONBOARD_LED_PIN);
}

void bsp_green_led_on(){

    LL_GPIO_SetOutputPin(ONBOARD_LED_PORT, ONBOARD_LED_PIN);
}

void bsp_green_led_off(){

    LL_GPIO_ResetOutputPin(ONBOARD_LED_PORT, ONBOARD_LED_PIN);
}
