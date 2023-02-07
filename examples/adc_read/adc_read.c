/**
 * @file adc_read.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief ADC demo
 * @version 0.1
 * @date 2023-01-30
 * 
 * @copyright Copyright (c) 2023
 * 
 * Reads some analog inputs on the Nucleo-L476RG
 * 
 * The purpose of this example is only to demonstrate the very basics of how to read from the ADC.
 * However in order to properly demonstrate this, the UART is used to send the converted data back
 * to the developers computer.
 * 
 * We read some external input pins, as well as some internal channels for temperature sensor, battery voltage
 * and reference voltage.
 * 
 */
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_adc.h>
#include <stm32l4xx_ll_usart.h>
#include <stm32l4xx_ll_utils.h>

#include <stm32l4xx_ll_cortex.h>
#include <stm32l4xx_ll_rcc.h>
#include "stm32l4xx_ll_bus.h"

volatile uint32_t counter = 0;

#define BLUE_LED_PORT          GPIOA
#define BLUE_LED_PIN           LL_GPIO_PIN_9

#define UART2_PORT             GPIOA
#define UART2_TX_PIN           LL_GPIO_PIN_2
#define UART2_RX_PIN           LL_GPIO_PIN_3

void SysTick_Handler(void)
{
    static uint_fast32_t counter = 0;
    counter++;

    // 1 Hz blinking
    if ((counter % 500) == 0)
        LL_GPIO_TogglePin(BLUE_LED_PORT, BLUE_LED_PIN);

}

/**
 * @brief Initialize GPIO
 * 
 */
void init_gpio() {

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(BLUE_LED_PORT, BLUE_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}

/**
 * @brief Initialize UART
 * 
 */
void init_uart() {
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

    LL_USART_SetBaudRate(USART2, SystemCoreClock, LL_USART_OVERSAMPLING_8, 9600);

    LL_USART_EnableDirectionRx(USART2);
    LL_USART_EnableDirectionTx(USART2);

    LL_USART_Enable(USART2);
}

/**
 * @brief Init ADC
 * 
 */
void init_adc() {

    // Pin A0 is connected to PA0 which supports ADC1_IN5

    // ADC conversions can be organized in to two groups, regular and injected
    // The regular conversion runs continously, while the injected conversion
    // must be triggered by software, or some external event.
    // Injected conversion has higher priority than regular and
    // is useful for e.g. motor control, where it allows
    // us to start conversion of the current measurements 
    // on specific favorable time instanst of the
    // transistor switching cycle.

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);

    /**
     * Configure the ADC clock
     * 
     */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);

    LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_SYSCLK);


    /**
     * @brief Software procedure to enable the ADC
     * 
     * From the reference manual for STM32l4, page 519.
     * 
     */
    LL_ADC_DisableDeepPowerDown(ADC1);

    LL_ADC_EnableInternalRegulator(ADC1);

    //LL_ADC_Disable(ADC1);
    while(__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE()); // Block here if ADC is enabled (it should not be)

    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV4);

    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);
    LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);
    LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_MODE_NONE);

    //LL_ADC_AWD_CH_VREFINT_REG

    //LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0}; 
    //LL_ADC_InitTypeDef ADC_InitStruct = {0};
    //LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};

    // Channel settings
    // Sequencer disabled is equivalent to sequencer of 1 rank, ADC conversion on only 1 channel.
    //LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE); 
    LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS);
    LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_1RANK);

    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);
    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);
    LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

    LL_ADC_DisableIT_EOC(ADC1);
    LL_ADC_DisableIT_EOS(ADC1);

    //LL_ADC_DELAY_INTERNAL_REGUL_STAB_US;
    //LL_mDelay(1);

    LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
    LL_ADC_SetOverSamplingDiscont(ADC1, LL_ADC_OVS_REG_DISCONT);

    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_TEMPSENSOR);
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_VBAT);
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_VREFINT);

    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_5);
    //LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_6);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2 , LL_ADC_CHANNEL_TEMPSENSOR);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_3 , LL_ADC_CHANNEL_VBAT);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_4 , LL_ADC_CHANNEL_VREFINT);

    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_5, LL_ADC_SAMPLINGTIME_24CYCLES_5);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SAMPLINGTIME_24CYCLES_5);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_VBAT, LL_ADC_SAMPLINGTIME_24CYCLES_5);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_VREFINT, LL_ADC_SAMPLINGTIME_24CYCLES_5);

    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_5, LL_ADC_SINGLE_ENDED);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SINGLE_ENDED);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_VBAT, LL_ADC_SINGLE_ENDED);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_VREFINT, LL_ADC_SINGLE_ENDED);


    LL_ADC_Enable(ADC1);
    while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0){}

    LL_ADC_REG_StartConversion(ADC1);
}

/**
 * @brief Calibrate ADC in order to improve accuracy
 * 
 */
void calibrate_adc() {

    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0);
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

int main(void) {

    static uint8_t seq_ctr = 0;

    init_gpio();
    init_uart();

    print_str("ADC read demo\r\n");
    init_adc();
    print_str("ADC initialization complete.\r\n");

    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    print_str("Sys clk: ");
    print_uint32(SystemCoreClock);
    print_str("\r\n");

    while(1) {

        LL_ADC_REG_StartConversion(ADC1);
        while(!LL_ADC_IsActiveFlag_EOC(ADC1));
        uint32_t data = LL_ADC_REG_ReadConversionData32(ADC1);

        print_str("Data: ");
        print_uint32(data);
        print_str("\r\n");
        switch(seq_ctr){
            case 0:{
                uint32_t volt = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300UL, data, LL_ADC_RESOLUTION_12B);
 
                print_str("Volt: ");
                print_uint32(volt);
                print_str(" mV");
            }
            break;
            case 1:{
                uint32_t temp = __LL_ADC_CALC_TEMPERATURE(3300UL, data, LL_ADC_RESOLUTION_12B);
                print_str("Temp: ");
                print_uint32(temp);
                print_str(" degC");
            }
            break;
            case 2:{
                uint32_t volt = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300UL, data, LL_ADC_RESOLUTION_12B);
  
                print_str("Batvolt: ");
                print_uint32(volt);
                print_str(" mV");
                break;
            }
            case 3:{
                uint32_t vref = __LL_ADC_CALC_VREFANALOG_VOLTAGE(data, LL_ADC_RESOLUTION_12B);
 
                print_str("Refvolt: ");
                print_uint32(vref);
                print_str(" mV");
                print_str("\r\n-----------------------------------------------------------------");
            }
            default:
        }


        seq_ctr++;
        if(seq_ctr > 3)
            seq_ctr = 0;

        print_str("\r\n");

        //LL_ADC_REG_StartConversion(ADC1);
        LL_mDelay(1000);
    }

    return 0;
}
