#include "net_connect.h"
#include "core_json.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//---------------------------------------------RAW DATA----------------------------------
extern UART_HandleTypeDef huart2;
uint8_t receive_buf[512];
uint8_t receive_temp[1];
uint16_t receive_count = 0;
uint8_t receive_start_flag = 0;
uint8_t receive_finish_flag = 0;
//----------------------------------------INTERRUPTS CALLBACK----------------------------------------
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//   if(huart->Instance==USART2)
//   {
//     receive_buf[receive_count] = receive_temp[0];
//     receive_count++;
//     receive_start_flag = 1;
//     HAL_UART_Receive_IT(&huart2,receive_temp,1);
//   }
// }
// void UARTReceiveIdleCallback(UART_HandleTypeDef* huart)
// {
//   if(huart->Instance==USART2)
//   {
//      if(receive_count>1)
//      {

//      }
//   }
// }

void NetDataReceiveHandle(void)
{
  uint8_t receive_data = 0;
  if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
  {
    HAL_UART_Receive(&huart2, &receive_data, 1, 1000);
    receive_buf[receive_count++] = receive_data;
    receive_start_flag = 1;
    receive_finish_flag = 0;
  }
}

//-----------------------------------------------CODE END----------------------------------------------

uint8_t parse_json_msg(uint8_t *json_msg, uint8_t json_len)
{
  uint8_t retval = 0;

  JSONStatus_t result;
  char query[] = "params.light";
  size_t queryLength = sizeof(query) - 1;
  char *value;
  size_t valueLength;
  result = JSON_Validate((const char *)json_msg, json_len);
  if (result == JSONSuccess)
  {
    result = JSON_Search((char *)json_msg, json_len, query, queryLength, &value, &valueLength);
    if (result == JSONSuccess)
    {
      char save = value[valueLength];
      value[valueLength] = '\0';
      printf("Found: %s %d-> %s\n", query, valueLength, value);
      value[valueLength] = save;
      retval = 0;
    }
    else
    {
      retval = 1;
    }
  }
  else
  {
    retval = 1;
  }
  return retval;
}

void NetConnectInit(void)
{
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  // HAL_UART_Receive_IT(&huart2,receive_temp,1);
}

void UARTReceiveClear(uint16_t len)
{
  memset(receive_buf, 0x00, len);
  receive_count = 0;
  receive_start_flag = 0;
  receive_finish_flag = 0;
}

/*
 * @retval status 0: Success, 1: Failed Receive 2: unmatched error
*/
void ESP8266SendCmd(uint8_t *cmd, uint8_t len, uint8_t *expect_data)
{
  uint8_t retval = 0;
  uint16_t count = 0;
  HAL_UART_Transmit(&huart2, cmd, len, 1000);
  // wait for slave emit signal approximate 1s
  while ((receive_start_flag == 0) && (count < 1000))
  {
    count++;
    HAL_Delay(1);
  }
  //Slave did not emit transmit signal
  if (count >= 1000)
  {
    retval = 1;
  }
  else
  {
    //after finish frame received 0.5s
    do
    {
      receive_finish_flag++;
      HAL_Delay(1);
    }while(receive_finish_flag < 500);
    retval = 2;
    if(strstr((const char *)receive_buf,expect_data))
    {
      retval = 0;
    }
  }
  UARTReceiveClear(receive_count);
  return retval;
}

