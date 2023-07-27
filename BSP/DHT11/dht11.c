#include "dht11.h"

void DHT11Init(void)
{
  DHT11ModeOUT();
  DHT11_DATA_OUT_HIGH;
}

void DHT11ModeIN(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Pin = DHT11_Pin;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

void DHT11ModeOUT(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}
//--------------------------START PROCEDURE BEGIN------------------------
// 主机拉低数据线>18ms,拉高20~40us,DHT11拉低40~50us，DHT11拉高40~50us
static void DHT11Start(void)
{
  DHT11ModeOUT();
  DHT11_DATA_OUT_LOW;
  HAL_Delay(20);
  DHT11_DATA_OUT_HIGH;
  mDelay_us(30);
}
static uint8_t DHT11DetectActivity()
{
  uint8_t Tick = 0;
  DHT11ModeIN();
  while (DHT11_DATA_IN && Tick <= 100)
  {
    Tick++;
    mDelay_us(1);
  }
  if (Tick > 100)
  {
    return 0;
  }
  else
  {
    Tick = 0;
  }
  while (!DHT11_DATA_IN && Tick <= 100)
  {
    Tick++;
    mDelay_us(1);
  }
  if (Tick > 100)
    return 0;
  return 1;
}
//--------------------------START PROCEDURE END------------------------

static uint8_t DHT11ReadBit(void)
{
  uint8_t Tick = 0;
  while (DHT11_DATA_IN && Tick <= 100)
  {
    Tick++;
    mDelay_us(1);
  }
  Tick = 0;
  while (!DHT11_DATA_IN && Tick <= 100)
  {
    Tick++;
    mDelay_us(1);
  }
  mDelay_us(40);
  if (DHT11_DATA_IN)
    return 1;
  else
    return 0;
}

static uint8_t DHT11ReadByte(void)
{
  uint8_t i, data;
  data = 0x00;
  for (i = 0; i < 8; i++)
  {
    data |= (DHT11ReadBit()) << (7 - i);
  }
  return data;
}
/**
 * @brief 获取温湿度
 * @param temp : 温度，
 * @param humi ：湿度
 * @return uint8_t 1:SUCCESS 0:FAULT 
 */
uint8_t DHT11ReadData(uint8_t *temp,uint8_t *humi)
{
  uint8_t msg[5];
  uint8_t i;
  DHT11Start();
  if(DHT11DetectActivity()==1)
  {
    for(i = 0; i < 5; i++){
      msg[i]=DHT11ReadByte();
    }
    if((msg[0]+msg[1]+msg[2]+msg[3])==msg[4])
    {
      //只需要整数部分
      *humi = msg[0];
      *temp = msg[2];
    }
    else{
      return 0;
    }
  }
  else {
    return 0;
  }
  return 1;
}
