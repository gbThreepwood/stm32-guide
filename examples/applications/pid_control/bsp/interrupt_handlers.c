//#include "stm32wlxx_ll_cortex.h"
//#include "stm32wlxx_ll_dma.h"
#include "stm32l4xx_ll_adc.h"
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_gpio.h"
//#include "stm32wlxx_ll_rtc.h"
//#include "stm32wlxx_ll_lptim.h"
//#include "stm32wlxx_ll_exti.h"

#include "qpc.h"

#include "bsp.h"
#include "bsp_gpio.h"

void SysTick_Handler(void){

    QTIMEEVT_TICK_X(0U, &l_SysTick_Handler); /* process time events for rate 0 */

    QV_ARM_ERRATUM_838869();

    //static uint_fast32_t counter = 0;
    //counter++;

    //if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_9)){
    //    // 1 Hz blinking
    //    if ((counter % 200) == 0)
    //        bsp_green_led_toggle();
    //        //LL_GPIO_TogglePin(ONBOARD_LED_PORT, ONBOARD_LED_PIN);
    //        //LL_GPIO_TogglePin(BLUE_LED_PORT, BLUE_LED_PIN);
    //}
    //else{
    //   if ((counter % 1500) == 0)
    //        bsp_green_led_toggle();
    //        //LL_GPIO_TogglePin(ONBOARD_LED_PORT, ONBOARD_LED_PIN);
    //}
}
