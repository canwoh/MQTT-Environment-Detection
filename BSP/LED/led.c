#include"led.h"


extern uint8_t LED_states;

void SetLedState(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,!LED_states);
}

