/**
 * @file main.c
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief PID temperature control demo
 * @version 0.1
 * @date 2024-03-27
 *
 * @copyright Copyright (c) 2024
 *
 * Measure the temperature of a bipolar transistor and control the heat
 * generated in the transistor using PWM.
 *
 * Make sure the VREF+ (AVDD) is connected to a voltage reference!
 *
 * Operation:
 * TIM2 triggers conversion from the ADC on regular intervals
 * DMA transfers the converted values to RAM
 * DMA HT and TC interrupts initiates conversion from raw values to physical values
 *
 * TIM4 generates PWM with controllable fsw and duty-cycle
 *
 * TIM2 triggers the UART to transfer a package of data on regular intervals. This includes
 * process-value (PV), and setpoint (SP).
 *
 * TIM2 triggers interrupt which executes the control loop on deterministic intervals
 * Control-loop computes suitable duty-cycle for PWM based on measured temperature and setpoint.
 *
 * Main loop handles the LC-display and encoder reading. Allowing the user to change parameters
 * such as proportional gain, integration and derivation time, bias, setpoint, etc.
 * TIM3 is responsible for counting the pulses from the encoder.
 *
 */
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_utils.h>

#include <stm32l4xx_ll_cortex.h>
#include <stm32l4xx_ll_rcc.h>
#include <stm32l4xx_ll_bus.h>

#include <stm32l4xx_ll_adc.h>
#include <stm32l4xx_ll_gpio.h>

#include <stm32l4xx_ll_tim.h>

#include "qpc.h"

#include "bsp/bsp_adc.h"
#include "bsp/bsp_dma.h"
#include "bsp/bsp_gpio.h"
#include "bsp/bsp_timer.h"
#include "bsp/bsp_uart.h"

#include "lib/encoder/libencoder.h"
#include "lib/hd44780/libhd44780.h"

#include "ledblink.h"

volatile uint32_t counter = 0;


bool get_encoder_button_state(){
    return !LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_9);
}

uint32_t get_encoder_timer_count(){
    return LL_TIM_GetCounter(TIM3);
}


int main(void) {


    bsp_init_gpio();
    bsp_init_uart();

    bsp_init_encoder_timer();

    libencoder_encoder_t encoder = {0};
    libencoder_encoderBtn_t encoderBtn = {0};
    libencoder_init(&encoder, &encoderBtn, get_encoder_button_state, get_encoder_timer_count);



    print_str("PID control demo\r\n");

    bsp_init_adc();

    print_str("ADC initialization complete.\r\n");

    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    print_str("Sys clk: ");
    print_uint32(SystemCoreClock);
    print_str("\r\n");

    libHD44780_ctx_t lcd_ctx = {0};
    libHD44780_Init(&lcd_ctx, &bsp_lcd_set_gpio_pins, &bsp_lcd_set_en_pin, 16, 2);

    libHD44780_Clear(&lcd_ctx);
    libHD44780_Puts(&lcd_ctx, 0, 0, "Hestfest");

    //bsp_green_led_on();

    static QEvt const *l_LEDblinkQSto[10];

    //QF_init();  /* initialize the framework and the underlying RT kernel */

    //LEDblink_ctor();
    ////Buzzer_dictionary_add();
    //QACTIVE_START(LEDblink_AO,            /* AO pointer to start */
    //            1U,                   /* unique QP priority of the AO */
    //            l_LEDblinkQSto,        /* storage for the AO's queue */
    //            Q_DIM(l_LEDblinkQSto), /* lenght of the queue [entries] */
    //            (void *)0,            /* stack storage (not used in QK) */
    //            0U,                   /* stack size [bytes] (not used in QK) */
    //            (QEvt *)0);           /* initial event (or 0) */

    //static BuzzerPlayRequestEvt smlPoolSto[10];
    //QF_poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    //return QF_run(); /* run the QF application */

    while(1) {

        LL_ADC_REG_StartConversion(ADC1);
        while(!LL_ADC_IsActiveFlag_EOC(ADC1));
        uint32_t data = LL_ADC_REG_ReadConversionData32(ADC1);

        uint32_t volt = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300UL, data, LL_ADC_RESOLUTION_12B);

        /*
         * TMP36
         * 10 mV/degC, 750 mV @ 25 degC
         */
        //uint32_t temp_mdegC = (volt - 500);

        //print_uint32(temp_mdegC);
        //print_str("\r\n");


        libencoder_poll(&encoder);


        if(libencoder_isEncTurnedLeft(&encoder)){
            print_str("Venstre\r\n");
        }

        if(libencoder_isEncTurnedRight(&encoder)){
            print_str("Høgre\r\n");
        }

        if(libencoder_isEncTurned(&encoder)){
            print_str("Rotert:");
            print_uint32(libencoder_getEncCount(&encoder));
            print_str("\r\n");
        }

        if(libencoder_isBtnPressed(&encoder)){
            print_str("Trykk\r\n");
        }

        if(libencoder_isBtnReleased(&encoder)){
            print_str("Slepp\r\n");
        }

        //if(libencoder_isBtnHeldDown(&encoder)){
        //    print_str("Hold\r\n");
        //}







        //uint32_t cnt = 0;// LL_TIM_GetCounter(TIM3);
        //print_uint32(cnt);

        //uint32_t volt = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300UL, data, LL_ADC_RESOLUTION_12B);
        //uint32_t temp = __LL_ADC_CALC_TEMPERATURE(3300UL, data, LL_ADC_RESOLUTION_12B);
        //uint32_t volt = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300UL, data, LL_ADC_RESOLUTION_12B);
        //uint32_t vref = __LL_ADC_CALC_VREFANALOG_VOLTAGE(data, LL_ADC_RESOLUTION_12B);

        LL_mDelay(10);
    }

    return 0;
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
