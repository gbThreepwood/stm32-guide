#include <stm32f767xx.h>

static volatile uint32_t systick_10ms_ticks = 0;

void SysTick_Handler(void) {
    systick_10ms_ticks++;
}

void gpio_pin_config(){
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // 0b00 = input. 0b01 = output. 0b10 = alternate. 0b11 = analog.
    GPIOB->MODER |= GPIO_MODER_MODER7_0;

    // 0 = push-pull. 1 = open-drain.
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT7);

    // 0b00 = low. 0b01 = medium. 0b10 = high. 0b11 = very high.
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR7_0 | GPIO_OSPEEDER_OSPEEDR7_1);

    // 0b00 = no. 0b01 = pull-up. 0b10 = pull-down. 0b11 = reserved.
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR7_0 | GPIO_PUPDR_PUPDR7_1);

}

/**
 * @brief A simple blocking delay function
 * 
 * @param msec blocking time in milliseconds
 */
void delay_ms(uint32_t msec) {
    uint32_t centisec = msec / 10;
    uint32_t dest_time = systick_10ms_ticks + centisec;

    // Avoid glitch if sys-tick interrupt is triggered here
    if(dest_time < centisec) {
        systick_10ms_ticks = 0;
        dest_time = centisec;
    }

    while(systick_10ms_ticks < dest_time) {
        asm("nop");
    }
}

int main(){

    gpio_pin_config();

    SysTick_Config(SystemCoreClock / 80);

    for(;;){ //ever

        // Set pin 7
        GPIOB->BSRR |= GPIO_BSRR_BS7;

        delay_ms(500);

        // Reset pin 7
        GPIOB->BSRR |= GPIO_BSRR_BR7;

        delay_ms(500);
    }
}
