#ifndef _RETARGET_H__
#define _RETARGET_H__

#include "stm32f1xx_hal.h"
#include "sys/stat.h"
#include "stdio.h"
#include "common_inc.h"
#include "stdarg.h"


//---------------------------User Code Begin--------------------------------
//void safe_printf(const char *fmt, ...);

//---------------------------User Code End--------------------------------
void RetargetInit(UART_HandleTypeDef *huart);

int _isatty(int fd);

int _write(int fd, char *ptr, int len);

int _close(int fd);

int _lseek(int fd, int ptr, int dir);

int _read(int fd, char *ptr, int len);

int _fstat(int fd, struct stat *st);


#endif //#ifndef _RETARGET_H__