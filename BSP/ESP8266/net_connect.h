#ifndef __NET_CONNECT_H__
#define __NET_CONNECT_H__

#include"common_inc.h"
#include"net_config.h"
// #define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))

// uint8_t receive_ringbuffer[64];



//void UARTReceiveIdleCallback(UART_HandleTypeDef* huart);
void NetDataReceiveHandle(void);
uint8_t NetConnectInit(void);
uint8_t ESP8266SendCmd(uint8_t *cmd, uint8_t len, uint8_t *expect_data);
uint8_t ESP8266SendMsg(void);
uint8_t ESP8266ReceiveMsg(void);

#endif