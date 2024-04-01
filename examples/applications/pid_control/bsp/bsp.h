#ifndef _BSP_H
#define _BSP_H

#include "qpc.h"

#define BSP_TICKS_PER_SEC 1000

//static QSpyId const l_SysTick_Handler = { 1U };
static const int l_SysTick_Handler = { 1U };

enum ApplicationSignals {
    DUMMY_SIG = Q_USER_SIG,
    LEDBLINK_TIMEOUT_SIG,
    MAX_SIG               /* the last signal */
};

#endif /* _BSP_H */
