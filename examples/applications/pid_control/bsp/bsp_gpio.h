#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H


#define BLUE_LED_PORT          GPIOA
#define BLUE_LED_PIN           LL_GPIO_PIN_9

#define ONBOARD_LED_PORT GPIOA
#define ONBOARD_LED_PIN LL_GPIO_PIN_5


void bsp_init_gpio();

void bsp_lcd_set_gpio_pins(uint8_t data, bool rs, bool rw);

void bsp_lcd_set_en_pin(bool en);

void bsp_green_led_toggle();
void bsp_green_led_on();
void bsp_green_led_off();

#endif /* _BSP_GPIO_H */
