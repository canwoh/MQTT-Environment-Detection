#ifndef __DELAY_US_H__
#define __DELAY_US_H__

#include"stm32f1xx_hal.h"

#define CPU_FREQUENCY_MHZ 72
#define mDelay_us(nTime) (help_delay_us(nTime))

void help_delay_us(__IO uint32_t nTime);



#endif