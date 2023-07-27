
#ifndef __DHT11_H__
#define __DHT11_H__

#include "main.h"


#define DHT11_DATA_OUT_LOW (HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,GPIO_PIN_RESET))
#define DHT11_DATA_OUT_HIGH (HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,GPIO_PIN_SET))
#define DHT11_DATA_IN       (HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin))


void DHT11Init(void);
uint8_t DHT11ReadData(uint8_t *temp,uint8_t *humi);
// uint8_t dht11_init (void);
// uint8_t dht11_read_data(uint8_t *temp,uint8_t *humi);

#endif 

