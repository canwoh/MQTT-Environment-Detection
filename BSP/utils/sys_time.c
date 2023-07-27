#include"sys_time.h"


uint32_t get_sysTick_us()
{
    uint32_t tickTime = HAL_GetTick()*1000 + (SysTick->VAL / CPU_FREQUENCY_MHZ);
    return tickTime;
}
uint32_t get_sysTick_ms()
{
    return HAL_GetTick();
}
uint32_t get_sysTick_us_part()
{
    return (SysTick->VAL / CPU_FREQUENCY_MHZ);
}