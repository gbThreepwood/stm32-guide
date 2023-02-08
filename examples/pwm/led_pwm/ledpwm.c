/**
 * @file ledpwm.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief LED PWM demo
 * @version 0.1
 * @date 2023-01-30
 * 
 * @copyright Copyright (c) 2023
 *
 * Blinks a LED connected to D8, and uses PWM to fade in and out the
 * intensity of a LED connected to pin D9 on the Nucleo-L476RG.
 * 
 */
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_cortex.h>
#include <stm32l4xx_ll_rcc.h>
#include <stm32l4xx_ll_bus.h>

#include <stm32l4xx_ll_tim.h>

#define BLUE_LED_PORT          GPIOA
#define BLUE_LED_PIN           LL_GPIO_PIN_9

#define GREEN_LED_PORT         GPIOC
#define GREEN_LED_PIN          LL_GPIO_PIN_7

#define RED_LED_PORT           GPIOB
#define RED_LED_PIN            LL_GPIO_PIN_6

void SysTick_Handler(void)
{
    static uint_fast32_t counter = 0;
    counter++;

    // 1 Hz blinking
    if ((counter % 500) == 0)
        LL_GPIO_TogglePin(BLUE_LED_PORT, BLUE_LED_PIN);

}

void init_gpio() {

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    LL_GPIO_SetPinMode(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_MODE_OUTPUT);
    //LL_GPIO_SetPinMode(GREEN_LED_PORT, GREEN_LED_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(RED_LED_PORT, RED_LED_PIN, LL_GPIO_MODE_OUTPUT);

    LL_GPIO_SetPinOutputType(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(GREEN_LED_PORT, GREEN_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(RED_LED_PORT, RED_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    // Pin D9 is connected to MCU pin PC7. TIM3_CH2 and TIM8_CH2 is available on this pin.
    // AF2 is TIM3, STM32L476RG datasheet page 94.
    LL_GPIO_SetPinMode(GREEN_LED_PORT, GREEN_LED_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GREEN_LED_PORT, GREEN_LED_PIN, LL_GPIO_AF_2);
}

void init_timer() {

    // Timer TIM3 is a 16-bit general purpose timer.

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    LL_TIM_SetClockSource(TIM3, LL_TIM_CLOCKSOURCE_INTERNAL);

    LL_TIM_SetPrescaler(TIM3, 2048);

    LL_TIM_SetCounterMode(TIM3, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM3, LL_TIM_CLOCKDIVISION_DIV1);

    LL_TIM_SetAutoReload(TIM3, 1000);
    LL_TIM_EnableARRPreload(TIM3);

    LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);

    LL_TIM_OC_ConfigOutput(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_SetCompareCH2(TIM3, 500);
    LL_TIM_OC_SetDeadTime(TIM3, 0);
    LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetIdleState(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_OCIDLESTATE_LOW);
    LL_TIM_OC_EnableFast(TIM3, LL_TIM_CHANNEL_CH2);
    LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH2);    

    LL_TIM_EnableAllOutputs(TIM3);

    LL_TIM_EnableCounter(TIM3);

}


void init_timer_cmsis() {

    RCC -> APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

	// Reset TIM3 configuration
	TIM3->CR1  = 0x0000;
	TIM3->CR2  = 0x0000;
	TIM3->CCER = 0x0000;

	// Set TIM3 prescaler
	TIM3->PSC = (uint16_t) 48 -1;

	// Set Auto-Reload to period
	TIM3->ARR = (uint16_t) 12000;

	// Enable Auto-Reload Preload register
	TIM3->CR1 |= TIM_CR1_ARPE;

	// Setup Input Capture
	TIM3->CCMR1 = 0x0000;
	TIM3->CCMR2 = 0x0000;

	// Setup PWM mode 1 output
	TIM3->CCMR1 |= (0x06 <<TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;
	TIM3->CCMR1 |= (0x06 <<TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;

	// Set default PWM values
	//TIM3->CCR1 = 3500;
	TIM3->CCR2 = 10000;

	// Enable Outputs
	TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;

	// Enable Main output
	TIM3->BDTR |= TIM_BDTR_MOE;

	// Enable TIM3
	TIM3->CR1 |= TIM_CR1_CEN;
}

int main(void) {

    init_gpio();
    init_timer();
    //init_timer_cmsis();

    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    while(1);

    return 0;
}
