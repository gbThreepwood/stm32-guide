#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

static void gpio_setup(void)
{
	/* Enable GPIOB clock. */
	rcc_periph_clock_enable(RCC_GPIOB);

	/* Set GPIO14 (in GPIO port B) to 'output push-pull'. */
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
}

int main(void)
{
	uint32_t i;

	gpio_setup();

	while (1) {
		/* Using API function gpio_toggle(): */
		gpio_toggle(GPIOB, GPIO14);	/* LED on/off */
		for (i = 0; i < 2000000; i++) /* Wait a bit. */
			__asm__("nop");

	}

	return 0;
}
