#include <Arduino.h>

#include <stm32f7xx_ll_gpio.h>
#include <stm32f7xx_ll_bus.h>

#define USER_LED_PORT           GPIOB
#define USER_LED1_PIN           LL_GPIO_PIN_0
#define USER_LED2_PIN           LL_GPIO_PIN_7
#define USER_LED3_PIN           LL_GPIO_PIN_14

void setup() {
  //pinMode(USER_LED_1, OUTPUT);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

  LL_GPIO_SetPinMode(USER_LED_PORT, USER_LED1_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(USER_LED_PORT, USER_LED1_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}

void loop() {

  LL_GPIO_TogglePin(USER_LED_PORT, USER_LED1_PIN);
  delay(100);
}
