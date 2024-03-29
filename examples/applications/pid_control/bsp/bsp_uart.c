

#include <stm32l4xx_ll_bus.h>
#include <stm32l4xx_ll_rcc.h>
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_usart.h>

#include "bsp_uart.h"


#define UART2_PORT             GPIOA
#define UART2_TX_PIN           LL_GPIO_PIN_2
#define UART2_RX_PIN           LL_GPIO_PIN_3


/**
 * @brief Initialize UART
 *
 */
void bsp_init_uart() {
    // From the Nucleo manual:
    // By default, the USART2 communication between the target STM32 and ST-LINK MCU is enabled.
    // PA2 for TX and PA3 for RX

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode(UART2_PORT, UART2_TX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(UART2_PORT, UART2_RX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(UART2_PORT, UART2_TX_PIN, LL_GPIO_AF_7);
    LL_GPIO_SetAFPin_0_7(UART2_PORT, UART2_RX_PIN, LL_GPIO_AF_7);


    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);

    LL_USART_SetOverSampling(USART2, LL_USART_OVERSAMPLING_8);

    LL_USART_SetBaudRate(USART2, SystemCoreClock, LL_USART_OVERSAMPLING_8, 115200);

    LL_USART_EnableDirectionRx(USART2);
    LL_USART_EnableDirectionTx(USART2);

    LL_USART_Enable(USART2);
}

/**
 * @brief
 *
 * Convert int to string representation in decimal number system
 * The caller should provide a pointer to the end of a buffer with
 * a size of at least 3*sizeof(selected int size) + 1 to always be on the safe side
 *
 * 2^32 = 4294967296
 *
 * @param x
 * @param s
 * @return char*
 */
char *intToStrDec(uint32_t x, char *s)
{
    *--s = 0; // Decrement pointer and NULL terminate string
    if (!x) *--s = '0'; // Add a zero in ASCII if x is zero
    for (; x; x/=10) *--s = '0'+x%10; // Loop as long as x is not equal to zero, divide by 10 and add the remainder as the current digit
    // (remember that x/=10 is performed after the body x%10)
    return s; // Return a pointer to the beginning of the string (this does not have to be the beginning of the buffer)
}

void print_uint32(uint32_t val) {

    char buf[3*sizeof(uint32_t)+1];

    char *str = intToStrDec(val, buf + sizeof(buf));

    while(*str){
        while(!LL_USART_IsActiveFlag_TC(USART2));
        LL_USART_TransmitData8(USART2, *str++);
    }
}

void print_str(char *str){
    while(*str){
        while(!LL_USART_IsActiveFlag_TC(USART2));
        LL_USART_TransmitData8(USART2, *str++);
    }
}

