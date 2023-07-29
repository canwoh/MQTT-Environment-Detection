#include "net_connect.h"
#include "core_json.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// if you want to use this mode,plz configure your wifi information and mqtt information
//-------------------------------CMD---------------------------------
#define CMD_AT_CWJAP "AT+CWJAP=\"" WIFI_NAME "\",\"" WIFI_PASSWD "\"\r\n"
#define CMD_AT_MODE "AT+CWMODE=1\r\n"
#define CMD_AT_MQTTUSERCFG "AT+MQTTUSERCFG=0,1,\""MQTT_CLIENT_ID"\",\""MQTT_USER_NAME"\",\""MQTT_PASSWD"\",0,0,\"\"\r\n"
#define CMD_AT_MQTTCONN "AT+MQTTCONN=0,\""BROKER_ADDR"\",1883,0\r\n"
#define CMD_AT_MQTTSUB "AT+MQTTSUB=0,\"" SUB_TOPIC "\",0\r\n"
#define CMD_AT_MQTTPUB "AT+MQTTPUB=0,\"" PUB_TOPIC "\",\"" JASON_FORMAT "\",0,0\r\n"

//-------------------------------CMD END------------------------------
//-----------------------------CMD String --------------------------------
static  char *AT_CWJAP_STR = CMD_AT_CWJAP;
  char *AT_MODE_STR = CMD_AT_MODE;
static  char *AT_MQTTUSERCFG_STR = CMD_AT_MQTTUSERCFG;
static  char *AT_MQTTCONN_STR = CMD_AT_MQTTCONN;
static  char *AT_MQTTSUB_STR = CMD_AT_MQTTSUB;
static  char *AT_MQTTPUB_STR = CMD_AT_MQTTPUB;

//----------------------------AT commands size------------------------------
static const uint16_t CWJAP_SIZE = strlen(CMD_AT_CWJAP);
 const uint16_t MODE_SIZE = strlen(CMD_AT_MODE);
static const uint16_t MQTTUSERCFG_SIZE = strlen(CMD_AT_MQTTUSERCFG);
static const uint16_t MQTTCONN_SIZE = strlen(CMD_AT_MQTTCONN);
static const uint16_t MQTTSUB_SIZE = strlen(CMD_AT_MQTTSUB);
static const uint16_t MQTTPUB_SIZE = strlen(CMD_AT_MQTTPUB);

//----------------------------------------DATA Control Variable----------------------------------
extern UART_HandleTypeDef huart2;
extern uint8_t temp;
extern uint8_t humi;

uint8_t receive_buf[256];
uint8_t receive_temp[1];
uint16_t receive_count = 0;
uint8_t receive_start_flag = 0;
uint8_t receive_finish_flag = 0;
uint8_t LED_states = 1;

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
void UARTReceiveClear(uint16_t len)
{
  memset(receive_buf, 0x00, len);
  receive_count = 0;
  receive_start_flag = 0;
  receive_finish_flag = 0;
}

uint8_t parse_json_msg(uint8_t *json_msg, uint8_t json_len)
{
  uint8_t retval = 0;

  JSONStatus_t result;
  char query[] = "params.switch";
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
      //set led state
      LED_states = value[0] - '0'; 
      printf("LED : %d\r\n", LED_states);
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

uint8_t NetConnectInit(void)
{
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  // HAL_UART_Receive_IT(&huart2,receive_temp,1);
  uint8_t isConnected = 0;
  isConnected = ESP8266SendCmd((uint8_t*)AT_MODE_STR,(uint8_t)MODE_SIZE,(uint8_t*)"OK");
  if(isConnected)
  {
    printf("Fail to set mode\r\n");
  }
  isConnected = ESP8266SendCmd((uint8_t*)AT_CWJAP_STR,(uint8_t)CWJAP_SIZE,(uint8_t*)"WIFI CONNECTED");
  if(isConnected)
  {
    printf("Fail to connect network\r\n");
  }
 UARTReceiveClear(254);
  HAL_Delay(1000);
  isConnected = ESP8266SendCmd((uint8_t*)AT_MQTTUSERCFG_STR,(uint8_t)MQTTUSERCFG_SIZE,(uint8_t*)"OK");
  if(isConnected)
  {
    printf("Fail to set mqtt configuration\r\n");
  }
  HAL_Delay(1000);
  //printf("debug:%s\r\n",receive_buf);
  printf(AT_MQTTCONN_STR);
  isConnected = ESP8266SendCmd((uint8_t*)AT_MQTTCONN_STR,(uint8_t)MQTTCONN_SIZE,(uint8_t*)"MQTT");
   if(isConnected)
  {
    printf("Fail to connect mqtt server\r\n");
  }
  printf("debug2\r\n");
  return isConnected;
}



/*
 * @retval status 0: Success, 1: Failed Receive 2: unmatched error
 */
uint8_t ESP8266SendCmd(uint8_t *cmd, uint8_t len, uint8_t *expect_data)
{
  uint8_t debug_flag = 0;
  uint8_t retval = 0;
  uint16_t count = 0;
  HAL_UART_Transmit(&huart2, cmd, len, 5000);
  // wait for slave emit signal approximate 1s
  while ((receive_start_flag == 0) && (count < 5000))
  {
    count++;
    HAL_Delay(1);
  }

  // Slave did not emit transmit signal
  if (count >= 5000)
  {
    retval = 1;
    debug_flag=2;
  }
  else
  {
    // after finish frame received 0.5s
    do
    {
      receive_finish_flag++;
      HAL_Delay(1);
    } while (receive_finish_flag < 200);
    retval = 2;
    if (strstr((const char *)receive_buf, (const char *)expect_data))
    {
      retval = 0;
      debug_flag=3;
    }
  }
  printf("debug_flag:%d\r\n",debug_flag);
  UARTReceiveClear(receive_count);
  return retval;
}

//Send temperature and humidity measurement to remote mqtt server
uint8_t ESP8266SendMsg(void)
{
  uint8_t retval = 0;
  uint16_t count = 0;
  uint8_t msg_buf[256];
  sprintf((char*)msg_buf,CMD_AT_MQTTPUB,temp,humi);
  HAL_UART_Transmit(&huart2, (unsigned char *)msg_buf,strlen((const char *)msg_buf), 1000);	
  printf("Send Msg : %s\r\n",msg_buf);
  while((receive_start_flag == 0)&&(count<500))	
	{
		count++;
		HAL_Delay(1);
	}
	if(count >= 500)	
	{
		retval = 1;	
	}
	else	
	{
		HAL_Delay(50);
		if(strstr((const char*)receive_buf, "OK"))	
		{
			retval = 0;	
		}
		else 
		{
			retval = 1;
      printf("failed to send message\r\n");
		}
	}
  UARTReceiveClear(receive_count);	
	return retval;
}

uint8_t ESP8266ReceiveMsg(void)
{
  uint8_t retval = 0;
  int msg_len=0;
  uint8_t msg[128];
  if(receive_start_flag==1)
  {
    do{
      receive_finish_flag++;
      HAL_Delay(1);
    }while(receive_finish_flag<5);
    if(strstr((const char*)receive_buf,"+MQTTSUBRECV:"))
    {
      sscanf((const char *)receive_buf,"+MQTTSUBRECV:0,\""SUB_TOPIC"\",%d,%s",&msg_len,msg);
      if(strlen((const char*)msg)==msg_len)
      {
        retval = parse_json_msg(msg,msg_len);
        printf("get msg : %s\r\n",msg);
      }else {
        retval = 1;
        printf("Error:unmatched 'MQTTSUBRECV' Length in message\r\n");
      }
    }else {
       retval = 1;
       printf("Error:unmatched 'MQTTSUBRECV'\r\n");
    }
  }
  else{
    retval = 1;
  }
  UARTReceiveClear(receive_count);
  return retval;
}
