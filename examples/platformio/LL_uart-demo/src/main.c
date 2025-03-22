/**
 * @file main.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief UART demo
 * @version 0.1
 * @date 2025-03-20
 * 
 * @copyright Copyright (c) 2025
 * 
 * Transmit data on one of the UARTs on the Nucleo-F767ZI
 * 
 */
#include <stm32f7xx_ll_gpio.h>
#include <stm32f7xx_ll_usart.h>
#include <stm32f7xx_ll_cortex.h>
#include <stm32f7xx_ll_rcc.h>
#include "stm32f7xx_ll_bus.h"
#include <stm32f7xx_ll_utils.h>
#include <stm32f7xx_ll_pwr.h>
#include <stm32f7xx_ll_system.h>

#include <string.h>

#define USER_LED_PORT          GPIOB
#define USER_LED1_PIN          LL_GPIO_PIN_0
#define USER_LED2_PIN          LL_GPIO_PIN_7
#define USER_LED3_PIN          LL_GPIO_PIN_14

#define USER_BTN_PORT          GPIOC
#define USER_BTN_PIN           LL_GPIO_PIN_13

#define UART3_PORT             GPIOD
#define UART3_TX_PIN           LL_GPIO_PIN_8
#define UART3_RX_PIN           LL_GPIO_PIN_9

void SysTick_Handler(void)
{
    static uint_fast32_t counter = 0;
    counter++;

    // 1 Hz blinking
    if ((counter % 500) == 0)
        LL_GPIO_TogglePin(USER_LED_PORT, USER_LED2_PIN);
}

void init_gpio() {

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    LL_GPIO_SetPinMode(USER_LED_PORT, USER_LED1_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(USER_LED_PORT, USER_LED1_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinMode(USER_LED_PORT, USER_LED2_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(USER_LED_PORT, USER_LED2_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}

void init_uart_using_basic_driver() {
    // From the Nucleo manual:
    // The USART3 interface available on PD8 and PD9 of the STM32 can be connected either to ST-LINK or to ST morpho connector.
    // The choice is changed by setting the related solder bridges. By default the USART3 communication is enabled between the
    // target STM32 and both the ST-LINK and the ST morpho connector.
    // PD8 USART3_TX
    // PD9 USART3_RX

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);

    LL_GPIO_SetPinPull(UART3_PORT, UART3_RX_PIN, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinPull(UART3_PORT, UART3_TX_PIN, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinSpeed(UART3_PORT, UART3_RX_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinSpeed(UART3_PORT, UART3_TX_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);

    LL_GPIO_SetPinMode(UART3_PORT, UART3_RX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(UART3_PORT, UART3_TX_PIN, LL_GPIO_MODE_ALTERNATE);

    LL_GPIO_SetPinOutputType(UART3_PORT, UART3_RX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(UART3_PORT, UART3_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetAFPin_8_15(UART3_PORT, UART3_RX_PIN, LL_GPIO_AF_7);
    LL_GPIO_SetAFPin_8_15(UART3_PORT, UART3_TX_PIN, LL_GPIO_AF_7);

    //LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_HSI);

    LL_USART_SetOverSampling(USART3, LL_USART_OVERSAMPLING_16);

    LL_USART_SetDataWidth(USART3, LL_USART_DATAWIDTH_8B);
    LL_USART_SetHWFlowCtrl(USART3, LL_USART_HWCONTROL_NONE);
    LL_USART_SetStopBitsLength(USART3, LL_USART_STOPBITS_1);
    LL_USART_SetParity(USART3, LL_USART_PARITY_NONE);
    LL_USART_SetTransferDirection(USART3, LL_USART_DIRECTION_TX_RX);
    LL_USART_SetBaudRate(USART3, SystemCoreClock, LL_USART_OVERSAMPLING_16, 115200);

    //LL_USART_EnableDirectionRx(USART3);
    //LL_USART_EnableDirectionTx(USART3);

    LL_USART_ConfigAsyncMode(USART3);

    LL_USART_Enable(USART3);
}


void init_uart_using_full_driver(void) {

    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);

    /*
     * USART3 GPIO Configuration
     *
     * PD8   ------> USART3_TX
     * PD9   ------> USART3_RX
     */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART configuration */
    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART3, &USART_InitStruct);
    LL_USART_ConfigAsyncMode(USART3);

    /* Enable USART */
    LL_USART_Enable(USART3);
}

void system_clock_config(void) {
    /* Configure flash latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
    if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_3) {
        while (1) {}
    }

    /* Configure voltage scaling */
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

    /* Configure HSI */
    LL_RCC_HSI_SetCalibTrimming(16);
    LL_RCC_HSI_Enable();
    while (LL_RCC_HSI_IsReady() != 1) {}

    /* Configure PLL */
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_8, 100, LL_RCC_PLLP_DIV_2);
    LL_RCC_PLL_Enable();
    while (LL_RCC_PLL_IsReady() != 1) {}

    /* Configure system prescalers */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {}

    /* Configure systick */
    LL_Init1msTick(100000000);
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SYSTICK_EnableIT();
    LL_SetSystemCoreClock(100000000);
    LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

int main(void) {
    init_gpio();

    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    init_uart_using_basic_driver();

    char demo_string[] = "Dette er ein test av Nucleo-L767ZI sin UART\r\n";

    while(1) {

        for(uint8_t i = 0; i < strlen(demo_string); i++){
            while(!LL_USART_IsActiveFlag_TC(USART3));
            LL_USART_TransmitData8(USART3, demo_string[i]);
        }

        LL_GPIO_TogglePin(USER_LED_PORT, USER_LED1_PIN); // Indikasjon på at main-løkka ikkje har hengt seg opp
        LL_mDelay(1800);
    };

    return 0;
}
