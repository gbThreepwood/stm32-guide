/**
 * @file timer_pwm.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief Timer pwm demo
 * @version 0.1
 * @date 2024-03-26
 *
 * @copyright Copyright (c) 2024
 *
 * Adjustable duty cycle PWM on a pin using TIM3 on STM32L476.
 * TIM3 is a 16-bit general purpose timer.
 *
 * We also blink the onboard LED using the SysTick interrupt. This
 * just serves as an indication that our MCU is running.
 *
 */
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_cortex.h>
#include <stm32l4xx_ll_rcc.h>
#include <stm32l4xx_ll_bus.h>
#include <stm32l4xx_ll_tim.h>
#include <stm32l4xx_ll_utils.h>
#include <stm32l4xx_ll_pwr.h>
#include <stm32l4xx_ll_system.h>

#define ONBOARD_LED_PORT GPIOA
#define ONBOARD_LED_PIN LL_GPIO_PIN_5

void SysTick_Handler(void){

  static uint_fast32_t cnt = 0;
  cnt++;

  if ((cnt % 1250) == 0){
    // Indicate that our MCU is alive
    LL_GPIO_TogglePin(ONBOARD_LED_PORT, ONBOARD_LED_PIN);
  }
}

void configure_system_clock();
void configure_timer();
void set_duty_cycle(uint16_t d);

void setup_everything(){

  configure_system_clock();
  configure_timer();

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

  // Configure the LED pin
  LL_GPIO_SetPinMode(ONBOARD_LED_PORT, ONBOARD_LED_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(ONBOARD_LED_PORT, ONBOARD_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);

  // Configure the PWM pin
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_6, LL_GPIO_AF_2);


}


int main(){

  setup_everything();

  uint16_t i = 0;

  for(;;){ // ever...

    set_duty_cycle(i);

    LL_mDelay(1000);
    i += 100;
    if(i > 1000)
      i = 0;

  }
}

void set_duty_cycle(uint16_t d){
  LL_TIM_OC_SetCompareCH1(TIM3, d);
}

void configure_timer(){

  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

  LL_TIM_SetCounterMode(TIM3, LL_TIM_COUNTERMODE_UP);
  LL_TIM_SetClockDivision(TIM3, LL_TIM_CLOCKDIVISION_DIV1);
  LL_TIM_SetRepetitionCounter(TIM1, 0);
  /*
   * ARR preload is used to buffer the ARR setting
   * so that the actual setting to the timer is only updated
   * on an update event. */
  //LL_TIM_DisableARRPreload(TIM3);
  LL_TIM_EnableARRPreload(TIM3);


  LL_TIM_SetClockSource(TIM3, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM3);

  //LL_TIM_EnableIT_UPDATE(TIM3);

  /*
   * Configure the PWM channel. Enable preload of
   * new compare settings, and set PWM mode.
   *
   */
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH1);
  LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);

  LL_TIM_OC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
  LL_TIM_OC_SetIdleState(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCIDLESTATE_HIGH);
  LL_TIM_OC_EnableFast(TIM3, LL_TIM_CHANNEL_CH1);

  LL_TIM_EnableAllOutputs(TIM3);
  LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);

  // Configure prescaler and counter auto-reload value in order
  // to set it for 1 seconds count interval, on a 80 Mhz system
  // clock.
  LL_TIM_SetPrescaler(TIM3, 80);
  LL_TIM_SetAutoReload(TIM3, 1000);

  //NVIC_SetPriority(TIM3_IRQn, 0);
  //NVIC_EnableIRQ(TIM3_IRQn);

  LL_TIM_EnableCounter(TIM3);

}


void configure_system_clock(){

  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_4){
  }

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is Ready */
  while(LL_RCC_HSI_IsReady() != 1){
  }

  LL_RCC_HSI_SetCalibTrimming(16);

  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 10, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_EnableDomain_SYS();
  LL_RCC_PLL_Enable();

   /* Wait till PLL is Ready */
  while(LL_RCC_PLL_IsReady() != 1){
  }
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System Clock is Ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL){
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  LL_Init1msTick(80000000); // 80 MHz

  LL_SetSystemCoreClock(80000000); // 80 MHz

  SysTick_Config(SystemCoreClock / 1000);
}


// TODO: These are here to get rid of some warnings, but I should find a better way to remove the warnings
void _close(void){
}

void _lseek(void){
}

void _read(void){
}

void _write(void){
}