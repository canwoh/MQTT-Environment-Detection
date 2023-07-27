#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

/*
    *@description :该获取Tick时间版本仅适用于采用TIM1当HAL_SYSTEM_Tick
*/
#define CPU_FREQUENCY_MHZ 72

#include"stm32f1xx_hal.h"

uint32_t get_sysTick_us(void);
uint32_t get_sysTick_ms(void);
uint32_t get_sysTick_us_part(void);



#endif