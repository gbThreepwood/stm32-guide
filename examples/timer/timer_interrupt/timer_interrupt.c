/**
 * @file timer_interrupt.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief Timer interrupt demo
 * @version 0.1
 * @date 2024-03-26
 *
 * @copyright Copyright (c) 2024
 *
 * Trigger interrupt on regular intervals using a timer.
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

  if ((cnt % 500) == 0){
    //LL_GPIO_TogglePin(ONBOARD_LED_PORT, ONBOARD_LED_PIN);
  }
}

void configure_system_clock();

void setup(){

  configure_system_clock();

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_GPIO_SetPinMode(ONBOARD_LED_PORT, ONBOARD_LED_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(ONBOARD_LED_PORT, ONBOARD_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);

  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
  LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
  LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
  LL_TIM_DisableARRPreload(TIM2);
  LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM2);
  LL_TIM_EnableIT_UPDATE(TIM2);

  // Configure prescaler and counter auto-reload value in order
  // to set it for 1 seconds count interval, on a 80 Mhz system
  // clock.
  LL_TIM_SetPrescaler(TIM2, 8000);
  LL_TIM_SetAutoReload(TIM2, 1000);

  // In real life you should probably put some thought into the setting of the IRQ priorites, but
  // here we just set it to zero.
  NVIC_SetPriority(TIM2_IRQn, 0);
  NVIC_EnableIRQ(TIM2_IRQn);

  LL_TIM_EnableCounter(TIM2);

}


void TIM2_IRQHandler(void){

  LL_GPIO_TogglePin(ONBOARD_LED_PORT, ONBOARD_LED_PIN);

  if (LL_TIM_IsActiveFlag_UPDATE(TIM2)) LL_TIM_ClearFlag_UPDATE(TIM2);
}


int main(){

  setup();

  if(LL_TIM_IsEnabledIT_UPDATE(TIM2)){}

  for(;;); // ever...
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