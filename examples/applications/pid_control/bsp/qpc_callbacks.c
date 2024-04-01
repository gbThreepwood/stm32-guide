
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_rtc.h"

#include "qpc.h"

#include "bsp.h"

#ifndef NVIC_PRIORITYGROUP_0
/**
 * @brief 0 bit for pre-emption priority and 4 bits for subpriority.
 */
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007)
/**
 * @brief 1 bit for pre-emption priority and 3 bits for subpriority.
 */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006)
/**
 * @brief 2 bit for pre-emption priority and 2 bits for subpriority.
 */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005)
/**
 * @brief 3 bit for pre-emption priority and 1 bits for subpriority.
 */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004)
/**
 * @brief 4 bit for pre-emption priority and 0 bits for subpriority.
 * This is the selected config for the group settings 0,1,2,3.
 */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003)
#endif

enum KernelUnawareISRs {
  KERNEL_UNAWARE_CMSIS_PRI_1,
  KERNEL_UNAWARE_CMSIS_PRI_2,
  KERNEL_UNAWARE_CMSIS_PRI_3,
  MAX_KERNEL_UNAWARE_CMSIS_PRI /* keep always last */
};

/* "kernel-unaware" interrupts can't overlap "kernel-aware" interrupts */
Q_ASSERT_COMPILE(MAX_KERNEL_UNAWARE_CMSIS_PRI <= QF_AWARE_ISR_CMSIS_PRI);

enum KernelAwareISRs {
  GPIOPORTA_PRI = QF_AWARE_ISR_CMSIS_PRI,
  SYSTICK_PRIO,
  MAX_KERNEL_AWARE_CMSIS_PRI  /* keep always last */
};

/* "kernel-aware" interrupts should not overlap the PendSV priority */
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF>>(8-__NVIC_PRIO_BITS)));


Q_DEFINE_THIS_FILE

#ifdef Q_SPY

/**
 * @brief Variable to hold the current QSpy tick time
 *
 */
QSTimeCtr QS_tickTime_;

/**
 * @brief Variable to hold the QSpy period between ticks
 *
 */
QSTimeCtr QS_tickPeriod_;

/* QSpy source IDs */
//static QSpyId const l_SysTick_Handler = { 0U };
//static QSpyId const l_GPIO_EVEN_IRQHandler = { 0U };

//static USART_TypeDef * const l_USART0 = ((USART_TypeDef *)(0x40010000UL));

//enum AppRecords { /* application-specific trace records */
//    BLINK_CONFIG_STAT = QS_USER,
//    COMMAND_STAT
//};

#endif


void qspy_add_dictionaries(){

    QS_USR_DICTIONARY(LED_STAT);

    /* global signals */
    QS_SIG_DICTIONARY(UART1_RX_IDLE_SIG, (void *)0);

    QS_OBJ_DICTIONARY(&l_SysTick_Handler);

    //BSP_led_dictionary_add();
}

/**
 * @brief Nested Vector Interrupt Controller (NVIC) priority grouping configuration.
 *
 * According to the reference manual the Nested Vector Interrupt Controller (NVIC) uses
 * 16 programmable priority levels (four bits of interrupt priority used). In total
 * the register holds 8 bits for priority setting for each interrupt, but the remaining
 * bits are ignored. A value of 0 (zero) is the highest priority while a value of 15 is
 * the lowest priority.
 *
 * The 4 bits of priority setting can be sub divided in to two categories:
 * - Preemption priority
 * - Sub priority
 *
 * From the QPC manual:
 * -------------------
 * The QP ports to ARMv7M or higher architectures never completely disables interrupts,
 * even inside the critical sections. On ARMv7M or higher architectures, the QP port disables interrupts
 * selectively using the BASEPRI register.
 *
 * The BASEPRI (Base Priority Mask Register) has the effect that when a certain non-zero 8-bit priority value
 * is set to this register, then it will disable all interrupts having the same or lower priority than the value set
 *
 * Only "kernel-aware" interrupts are allowed to call QP services. "Kernel-unaware" interrupts are not allowed
 * to call any QP services and they can communicate with QP only by triggering a "kernel-aware" interrupt
 * (which can post or publish events).
 *
 * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
 * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
 *
 * Consequences
 * ------------
 * To avoid any surprises, the call to NVIC_SetPriorityGrouping(0U) is recommended.
 * this is equivalent to the call: NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4)
 */
void QF_onStartup(void) {
    /* Set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* Assign all priority bits for preemption-prio, and none to sub-prio. */
    NVIC_SetPriorityGrouping(0U);

    /**
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
    NVIC_SetPriority(SysTick_IRQn,   QF_AWARE_ISR_CMSIS_PRI);

    /* enable IRQs... */
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    NVIC_EnableIRQ(USART1_IRQn);

    /*
     * The dictionaries should be added after UART and DMA is enabled (including their IRQs)
     * The dictionary size is getting considerable and if we add them before, we will overflow
     * the UART TX buffer.
     */
    qspy_add_dictionaries();


    //LL_SYSTICK_EnableIT(); /* enable systick ISR just before starting to run QF */ SysTick_Config enables the interrupt.
}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/

/**
 * @brief QV idle callback function
 *
 * From documentation in the QPC framework we have:
 *
 * Called by the cooperative QV kernel (from QF_run()) when the scheduler detects
 * that no events are available for active objects (the idle condition)
 *
 * The QV_onIdle() callback is called with interrupts disabled, because the
 * determination of the idle condition might change by any interrupt posting
 * an event. QV_onIdle() must internally enable interrupts, ideally
 * atomically with putting the CPU to the power-saving mode.
 */
void QV_onIdle(void) {

#ifdef Q_SPY

    QF_INT_ENABLE();

    uint8_t rx_byte = 0;
    // Serial port read

    #if defined Q_SPY_UART_UART1
    if (lwrb_read(&usart1_rx_rb, &rx_byte, 1) == 1) { // Check if data is available, and read one byte if it is.
        QS_RX_PUT(rx_byte);
    }
    #elif defined Q_SPY_UART_UART2
    if (lwrb_read(&usart2_rx_rb, &rx_byte, 1) == 1) { // Check if data is available, and read one byte if it is.
        QS_RX_PUT(rx_byte);
    }
    #endif

    QS_rxParse();  /* parse all the received bytes */


    //uint16_t b;

    // TODO: Try to improve the UART transmission efficiency by writing multiple bytes to the buffer
    // by using QS_getBlock()

    uint16_t number_of_bytes = 50; // The assigned value is the maximum number of bytes we are willing to accept
    uint8_t const * tx_bytes;

    QF_INT_DISABLE();
    if((tx_bytes = QS_getBlock(&number_of_bytes)) != NULL){

        QF_INT_ENABLE();

        #if defined Q_SPY_UART_UART1

        lwrb_write(&usart1_tx_rb, tx_bytes, number_of_bytes);   /* Write data to transmit buffer */
        usart1_start_dma_tx();

        #elif defined Q_SPY_UART_UART2

        lwrb_write(&usart2_tx_rb, tx_bytes, number_of_bytes);   /* Write data to transmit buffer */
        usart2_start_dma_tx();

        #endif
        //QF_INT_DISABLE();
    }

    //// QS_getByte() returns a uint16_t. The low 8 bits are the data, while
    //// the higher bits are used to indicate empty buffer.
    //while ((b = QS_getByte()) != QS_EOD) { /* while not End-Of-Data... */
    //    QF_INT_ENABLE();

    //    uint8_t data = b & 0x00FF;
    //    #if defined Q_SPY_UART_UART1

    //    lwrb_write(&usart1_tx_rb, &data, 1);   /* Write data to transmit buffer */
    //    usart1_start_dma_tx();
    //
    //    //while(20 >= lwrb_get_free(&usart1_tx_rb)){} /* Wait until UART has had time to transmit some data using DMA */

    //    #elif defined Q_SPY_UART_UART2
    //    lwrb_write(&usart2_tx_rb, &data, 1);   /* Write data to transmit buffer */
    //    usart2_start_dma_tx();
    //    #endif
    //    QF_INT_DISABLE();
    //}

    //QF_INT_ENABLE();

#elif defined NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M MCU.
    */
    QV_CPU_SLEEP();  /* atomically go to sleep and enable interrupts */
#else
    QF_INT_ENABLE(); /* just enable interrupts */
#endif
}

/*..........................................................................*/




void application_q_spy_init() {

    if (QS_INIT((void *)0) == 0) { /* initialize the QS software tracing */
        Q_ERROR();
    }


    /* Setup the QS filters... */
    QS_GLB_FILTER(-QS_ALL_RECORDS);
    QS_GLB_FILTER(-QS_QF_TICK);    /* exclude the clock tick */

    QS_GLB_FILTER(QS_EQ_RECORDS);

    //QS_GLB_FILTER(-QS_QF_TICK);
    //QS_GLB_FILTER(-QS_EQ_RECORDS);
    //QS_GLB_FILTER(-QS_AO_RECORDS);
    //QS_GLB_FILTER(-QS_TE_RECORDS);
    //QS_GLB_FILTER(QS_SM_RECORDS);
    //QS_GLB_FILTER(-QS_UA_RECORDS);

    QS_GLB_FILTER(QS_U0_RECORDS);
    QS_GLB_FILTER(QS_U1_RECORDS);



    /* Setup QS local filter */
    //QS_LOC_FILTER(-QS_AO_IDS);
    QS_LOC_FILTER(-1);             /* turn the heartbeat AO (which has priority 1) OFF */
    QS_LOC_FILTER(-2);            /* turn the console AO (which has priority 2) OFF */
}

/**
 * @brief
 *
 * QP 7.3.0 name chage from Q_onAssert() to Q_onError().
 *
 * The Q_onAssert function should be called by the assert_failed function defined
 * in the microcontroller "startup.s" file. The assert_failed function should
 * be defined in assembly so that it is able to re-set the stack pointer, in case
 * it is corrupted due to the assertion failure which caused the assert_failed
 * function to be called in the first place.
 *
 * @param module
 * @param loc
 * @return Q_NORETURN
 */
Q_NORETURN Q_onError(char const * const module, int_t const id) {
    /*
    * NOTE: add here your application-specific error handling
    */
    (void)module;
    (void)id;
    QS_ASSERTION(module, id, 10000U); /* report assertion to QS */

    #ifdef DEBUG
    __ASM volatile("BKPT #01"); /* Make the debugger break here */
    while(1){}
    #else
    NVIC_SystemReset();
    #endif
}



/* QS callbacks ============================================================*/
#ifdef Q_SPY
/*..........................................................................*/
uint8_t QS_onStartup(void const *arg) {

    static uint8_t qsTxBuf[2*1024]; /* buffer for QS transmit channel */
    static uint8_t qsRxBuf[100];    /* buffer for QS receive channel */

    QS_initBuf  (qsTxBuf, sizeof(qsTxBuf));
    QS_rxInitBuf(qsRxBuf, sizeof(qsRxBuf));

    QS_tickPeriod_ = SystemCoreClock / BSP_TICKS_PER_SEC;
    QS_tickTime_ = QS_tickPeriod_; /* to start the timestamp at zero */

    return 1U; /* return success */
}
/*..........................................................................*/
void QS_onCleanup(void) {

}
/*..........................................................................*/

//#if !defined(Q_UTEST)






QSTimeCtr QS_onGetTime(void) {  /* NOTE: invoked with interrupts DISABLED */

    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) { /* not set? */
        return QS_tickTime_ - (QSTimeCtr)SysTick->VAL;
    }
    else { /* the rollover occured, but the SysTick_ISR did not run yet */
        return QS_tickTime_ + QS_tickPeriod_ - (QSTimeCtr)SysTick->VAL;
    }

}
//#endif

/*..........................................................................*/
void QS_onFlush(void) {
    //uint16_t b;

    QF_INT_DISABLE();

    // TODO: Try to improve the UART transmission efficiency by writing multiple bytes to the buffer
    // by using QS_getBlock()

    uint16_t number_of_bytes = 50; // The assigned value is the maximum number of bytes we are willing to accept
    uint8_t const * tx_bytes;

    QF_INT_DISABLE();
    if((tx_bytes = QS_getBlock(&number_of_bytes)) != NULL){

        QF_INT_ENABLE();

        #if defined Q_SPY_UART_UART1

        lwrb_write(&usart1_tx_rb, tx_bytes, number_of_bytes);   /* Write data to transmit buffer */
        usart1_start_dma_tx();

        #elif defined Q_SPY_UART_UART2

        lwrb_write(&usart2_tx_rb, tx_bytes, number_of_bytes);   /* Write data to transmit buffer */
        usart2_start_dma_tx();

        #endif
        //QF_INT_DISABLE();
    }




    // QS_getByte() returns a uint16_t. The low 8 bits are the data, while
    // the higher bits are used to indicate empty buffer.
    //while ((b = QS_getByte()) != QS_EOD) { /* while not End-Of-Data... */
    //    QF_INT_ENABLE();

    //    uint8_t data = b & 0x00FF;

    //    #if defined Q_SPY_UART_UART1

    //    lwrb_write(&usart1_tx_rb, &data, 1);   /* Write data to transmit buffer */
    //    usart1_start_dma_tx();

    //    //while(20 >= lwrb_get_free(&usart1_tx_rb)){} /* Wait until UART has had time to transmit some data using DMA */

    //    #elif defined Q_SPY_UART_UART2
    //    lwrb_write(&usart2_tx_rb, &data, 1);   /* Write data to transmit buffer */
    //    usart2_start_dma_tx();
    //    #endif
    //    QF_INT_DISABLE();
    //}

    //QF_INT_ENABLE();
}
/*..........................................................................*/
/*! callback function to reset the target (to be implemented in the BSP) */
void QS_onReset(void) {
    NVIC_SystemReset();
}
/*..........................................................................*/
/*! callback function to execute a user command (to be implemented in BSP) */
void QS_onCommand(uint8_t cmdId, uint32_t param1, uint32_t param2, uint32_t param3)
{

    Q_UNUSED_PAR(cmdId);
    Q_UNUSED_PAR(param1);
    Q_UNUSED_PAR(param2);
    Q_UNUSED_PAR(param3);

    //QS_BEGIN_ID(COMMAND_STAT, 0U) /* app-specific record */
    //    QS_U8(2, cmdId);
    //    QS_U32(8, param1);
    //    QS_U32(8, param2);
    //    QS_U32(8, param3);
    //QS_END()

    //if (cmdId == 10U) {
    //    Q_ERROR();
    //}
    //else if (cmdId == 11U) {
    //    assert_failed("QS_onCommand", 123);
    //}
}





///*--------------------------------------------------------------------------*/
//void QS_onTestSetup(void) {
//}
///*..........................................................................*/
//void QS_onTestTeardown(void) {
//}
//
//
///*..........................................................................*/
///* host callback function to "massage" the event, if necessary */
//void QS_onTestEvt(QEvt *e) {
//#ifdef Q_HOST  /* is this test compiled for a desktop Host computer? */
//#else /* this test is compiled for an embedded Target system */
//#endif
//
//    /* unused parameters... */
//    (void)e;
//}
///*..........................................................................*/
///*! callback function to output the posted QP events (not used here) */
//void QS_onTestPost(void const *sender, QActive *recipient,
//                   QEvt const *e, bool status)
//{
//    Q_UNUSED_PAR(sender);
//    Q_UNUSED_PAR(recipient);
//    Q_UNUSED_PAR(e);
//    Q_UNUSED_PAR(status);
//}


//void QS_onTestLoop() {
//
//    QS_rxPriv_.inTestLoop = true;
//    while (QS_rxPriv_.inTestLoop) {
//
//        /* toggle an LED on and then off (not enough LEDs, see NOTE02) */
//        //GPIOA->BSRR |= (LED_LD2);        /* turn LED[n] on  */
//        //GPIOA->BSRR |= (LED_LD2 << 16);  /* turn LED[n] off */
//
//        QS_rxParse();  /* parse all the received bytes */
//
//        QF_INT_DISABLE();
//
//        uint16_t b;
//
//        // TODO: Try to improve the UART transmission efficiency by writing multiple bytes to the buffer
//        // by using QS_getBlock()
//
//        // QS_getByte() returns a uint16_t. The low 8 bits are the data, while
//        // the higher bits are used to indicate empty buffer.
//        while ((b = QS_getByte()) != QS_EOD) { /* while not End-Of-Data... */
//            QF_INT_ENABLE();
//
//            uint8_t data = b & 0x00FF;
//            #if defined Q_SPY_UART_UART1
//            lwrb_write(&usart1_tx_rb, &data, 1);   /* Write data to transmit buffer */
//            usart1_start_dma_tx();
//            #elif defined Q_SPY_UART_UART2
//            lwrb_write(&usart2_tx_rb, &data, 1);   /* Write data to transmit buffer */
//            usart2_start_dma_tx();
//            #endif
//            QF_INT_DISABLE();
//        }
//
//        QF_INT_ENABLE();
//
//        //if ((USART2->ISR & (1U << 7)) != 0) {  /* is TXE empty? */
//        //    QF_INT_DISABLE();
//        //    uint16_t b = QS_getByte();
//        //    QF_INT_ENABLE();
//
//        //    if (b != QS_EOD) {  /* not End-Of-Data? */
//        //        USART2->TDR = (b & 0xFFU);  /* put into the DR register */
//        //    }
//        //}
//    }
//    /* set inTestLoop to true in case calls to QS_onTestLoop() nest,
//    * which can happen through the calls to QS_TEST_PAUSE().
//    */
//    QS_rxPriv_.inTestLoop = true;
//
//}


#endif /* Q_SPY */