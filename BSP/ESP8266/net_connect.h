#ifndef __NET_CONNECT_H__
#define __NET_CONNECT_H__

#include"common_inc.h"
#include"net_config.h"
// #define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))

// uint8_t receive_ringbuffer[64];



//void UARTReceiveIdleCallback(UART_HandleTypeDef* huart);
void NetDataReceiveHandle(void);
void NetConnectInit(void);
uint8_t parse_json_msg(uint8_t *json_msg,uint8_t json_len);
void ESP8266SendCmd(uint8_t *cmd, uint8_t len, uint8_t *expect_data);


#endif