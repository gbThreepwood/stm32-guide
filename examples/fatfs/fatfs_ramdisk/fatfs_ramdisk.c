/**
 * @file fatfs_ramdisk.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief Basic FatFS usage demo
 * @version 0.1
 * @date 2023-01-30
 * 
 * @copyright Copyright (c) 2023
 * 
 * Use the FatFS driver to store and retrieve data from RAM on the Nucleo-L476RG
 * 
 */
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_cortex.h>
#include <stm32l4xx_ll_rcc.h>
#include <stm32l4xx_ll_bus.h>
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_pwr.h"
#include <stm32l4xx_ll_dac.h>
#include <stm32l4xx_ll_utils.h>

#include "ff.h"
#include "diskio.h"
#include "ffconf.h"

#define BLUE_LED_PORT          GPIOA
#define BLUE_LED_PIN           LL_GPIO_PIN_9

/* FatFs object */
FATFS FatFs;

/* File object */
FIL fil;

void SysTick_Handler(void)
{
    static uint_fast32_t counter = 0;
    counter++;

    // 1 Hz blinking
    if ((counter % 500) == 0)
        LL_GPIO_TogglePin(BLUE_LED_PORT, BLUE_LED_PIN);

}

void init_clock(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
    while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_4)
    {
    }

    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    LL_RCC_HSI_Enable();    
     /* Wait till HSI is ready */
    while(LL_RCC_HSI_IsReady() != 1)
    {   
    }

    LL_RCC_HSI_SetCalibTrimming(16);
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 10, LL_RCC_PLLR_DIV_2);
    LL_RCC_PLL_EnableDomain_SYS();
    LL_RCC_PLL_Enable();

     /* Wait till PLL is ready */
    while(LL_RCC_PLL_IsReady() != 1)
    {   
    }

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

     /* Wait till System clock is ready */
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {   
    }

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    LL_Init1msTick(80000000);   
    LL_SetSystemCoreClock(80000000);
}

void init_gpio() {

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    //LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    //LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    LL_GPIO_SetPinMode(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_MODE_OUTPUT);

    LL_GPIO_SetPinOutputType(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}


/* Get current time */
DWORD get_fattime (void){
    return 0;
}

int main(void) {

    init_clock(); // Increase the clock speed in order to increase the output waveform frequency

    init_gpio();


    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

	/* Free and total space */
	//uint32_t total, free;

    /* Mount drive */
	if (f_mount(&FatFs, "0:", 1) == FR_OK) {

        /* Try to open file */
		if (f_open(&fil, "0:testfile.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE) == FR_OK) {

            /* Try to write to file */
			if (f_puts("Some text to add to the file\n", &fil) > 0) {

				//if (TM_FATFS_DriveSize(&total, &free) == FR_OK) {
				//	/* Data for drive size are valid */
				//}

            }

            /* Close file */
			f_close(&fil);
        }

        /* Unmount drive */
        f_mount(0, "0:", 1);
    }

    for(;;){

    }

    return 0;
}

