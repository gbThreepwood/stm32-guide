#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H


#define BLUE_LED_PORT          GPIOA
#define BLUE_LED_PIN           LL_GPIO_PIN_9

#define ONBOARD_LED_PORT GPIOA
#define ONBOARD_LED_PIN LL_GPIO_PIN_5


void bsp_init_gpio();

#endif /* _BSP_GPIO_H */

