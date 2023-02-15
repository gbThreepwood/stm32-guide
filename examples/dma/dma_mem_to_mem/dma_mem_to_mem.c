/**
 * @file ledblink.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief DMA demo
 * @version 0.1
 * @date 2023-01-30
 * 
 * @copyright Copyright (c) 2023
 * 
 * Demonstrate memory to memory DMA on the Nucleo-L476RG
 * 
 * This example is just a silly demo of moving some data around in the memory.
 * The only purpose is to demonstrate configuration and use of the DMA with no
 * emphasis on using it for anything useful.
 * 
 * * Moving a uint32_t from a source to a destination
 * * Moving a larger array from a source to a destination
 * 
 * Use the debugger to inspect the variables in order to verify the operation of the DMA.
 * UART printing is not included in the example since we do not want to make it too complex.
 * 
 */
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_cortex.h>
#include <stm32l4xx_ll_rcc.h>
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_utils.h"


/**
 * \brief           Calculate length of statically allocated array
 */
#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))

#define BLUE_LED_PORT          GPIOA
#define BLUE_LED_PIN           LL_GPIO_PIN_9

static uint32_t src_32_bit_demo_variable = 42;
static uint32_t dst_32_bit_demo_variable = 0;

#define DEMO_ARRAY_SIZE 8

/**
 * @brief Initialize the src demo array
 *
 * This is a non-standard GCC extension for array initialization. If the array is large
 * the *.elf will also be large. 
 */
//static uint32_t src_array_demo[DEMO_ARRAY_SIZE] = {[0 ... DEMO_ARRAY_SIZE - 1] = 23};

/**
 * @brief Initialize the src demo array with some spesific data
 * 
 * It is possible to configure the DMA to transfer the data at a single source address
 * to many destination addresses. This spesific data allows us to more esily verify that
 * the DMA is copying word by word from src to dst.
 *
 */
static uint32_t src_array_demo[DEMO_ARRAY_SIZE] = {
    0xB16B00B5,
    0xFAFAFAFA,
    0xB16B00B5,
    0xB16B00B5,
    0xDEADBEEF,
    0xDEADBEEF,
    0xDEADBEEF,
    0xDEADBEEF
};

static uint32_t dst_array_demo[DEMO_ARRAY_SIZE] = {0};

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
    //LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    //LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    LL_GPIO_SetPinMode(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_MODE_OUTPUT);

    LL_GPIO_SetPinOutputType(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}

void init_dma_array_transfer_demo() {

    NVIC_SetPriority(DMA1_Channel3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3, LL_DMA_DIRECTION_MEMORY_TO_MEMORY);
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_NORMAL);

    // In memory to memory transfer peripheral increment is memory source increment
    // You should also try NOINCREMENT and observe the effect
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PERIPH_INCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MEMORY_INCREMENT);

    // In memory to memory transfer peripheral size is memory source size
    // You should try to reduce the size from WORD to HALFWORD or BYTE and observe the effect
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PDATAALIGN_WORD);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MDATAALIGN_WORD);

    // The priority does not matter in this demo
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PRIORITY_VERYHIGH);

    // Try to reduce the length and observe that the array is only partially copied
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, ARRAY_LEN(src_array_demo));

    LL_DMA_SetM2MSrcAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)&src_array_demo);
    LL_DMA_SetM2MDstAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)&dst_array_demo);

    // Enable half transfer (HT), and transfer complete (TC) interrupts
    //
    // Breakpoints (or watchpoints) inside the HT ISR will not easily allow us to observe that the DMA
    // is half way through the transfer, since the DMA will continue in the background
    // even when GDB breaks the main program.
    //
    // Additionally watchpoints are also no use since they only detect when the CPU tries
    // to change the variable, not when the DMA or some other peripheral is responsible for the change.
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_3);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);

    // The transfer starts when the channel is enabled
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
}

void init_dma_single_4_byte_transfer_demo() {

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    //LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_MEMORY_TO_MEMORY);

    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_MEMORY_TO_MEMORY);

    /**
     * @brief Set DMA mode 
     * 
     * The circular mode must not be used with memory to memory transfers.
     */
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_NORMAL);

    //LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_NOINCREMENT);

    //LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_BYTE);

    /**
     * @brief Set the memory size (8, 16, or 32 bit)
     *
     * The STM32 is 32-bit, hence a word is 32-bit. 
     */
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_BYTE);

    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

    /**
     * @brief Sets the length of the data we are going to transfer
     * 
     */
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, sizeof(uint32_t));

    //LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1, &src_demo_variable, &dst_demo_variable, LL_DMA_DIRECTION_MEMORY_TO_MEMORY);
    LL_DMA_SetM2MSrcAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)&src_32_bit_demo_variable);
    LL_DMA_SetM2MDstAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)&dst_32_bit_demo_variable);

    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMA_REQUEST_0);
    
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
}

void restart_dma_single_4_byte_transfer_demo() {

    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_ClearFlag_HT1(DMA1);
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_ClearFlag_GI1(DMA1);
    LL_DMA_ClearFlag_TE1(DMA1);

    LL_DMA_SetM2MDstAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)&dst_32_bit_demo_variable);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, sizeof(uint32_t));
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
}

int main(void) {
    init_gpio();

    init_dma_single_4_byte_transfer_demo();
    init_dma_array_transfer_demo();

    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    while(1){

        for(uint8_t i = 0; i < 40; i++){
            src_32_bit_demo_variable += 2;
            LL_mDelay(100);
            restart_dma_single_4_byte_transfer_demo();
        }
    }

    return 0;
}

void DMA1_Channel3_IRQHandler(void)
{

  /* Check half-transfer complete interrupt */
  if (LL_DMA_IsEnabledIT_HT(DMA1, LL_DMA_CHANNEL_3) && LL_DMA_IsActiveFlag_HT3(DMA1)) {
      LL_DMA_ClearFlag_HT3(DMA1);             /* Clear half-transfer complete flag */
  }

  /* Check transfer-complete interrupt */
  if (LL_DMA_IsEnabledIT_TC(DMA1, LL_DMA_CHANNEL_3) && LL_DMA_IsActiveFlag_TC3(DMA1)) {
      LL_DMA_ClearFlag_TC3(DMA1);             /* Clear transfer complete flag */
  }

}