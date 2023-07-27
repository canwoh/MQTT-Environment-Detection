#include "errno.h"
#include "sys/time.h"
#include "utils/retarget.h"
#include "usart.h"

#if !defined(OS_USE_SEMIHOSTING)

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

//--------------------------------USER CODE BEGIN--------------------------------

//--------------------------------Work for FreeRTOS------------------------------
//FreeRtos exclusive Debug Print
// void debug_printf(const char *fmt, ...)
// {   
//     va_list args; 
//     if(xSemaphoreTake(semDebug,5)==pdFALSE)return;
//     va_start(args,fmt); 
//     vprintf(fmt,args); 
//     va_end(args); 
//     xSemaphoreGive(semDebug);   
// }
//uint32 uxTaskGetStackHighWaterMark(TaskHandle); //help for finding the suitable stack



// extern osSemaphoreId debug_print;

//!!!!!!!!!!!!!
//Don't use this function in critical section!!! 
//it is nessary to use the binary semaphore to support the function running 
/*
  osSemaphoreId debug_print;
  osSemaphoreDef(debug_print);
  debug_print = osSemaphoreNew(1,1,osSemaphore(debug_print));
*/
// void safe_printf(const char *fmt, ...)
// {
//     if(osSemaphoreAcquire(debug_print,10)==pdFALSE)return;
//     va_list args; 
//     va_start(args,fmt); 
//     vprintf(fmt,args); 
//     va_end(args); 
//     osSemaphoreRelease(debug_print);     
// }

//--------------------------------USER CODE END--------------------------------
UART_HandleTypeDef *gHuart;
void RetargetInit(UART_HandleTypeDef *huart)
{
    gHuart = huart;

    /* Disable I/O buffering for STDOUT stream, so that
     * chars are sent out as soon as they are printed. */
    setvbuf(stdout, NULL, _IONBF, 0);
}

int _isatty(int fd)
{
    if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
        return 1;

    errno = EBADF;
    return 0;
}

int _write(int fd, char *ptr, int len)
{
    HAL_StatusTypeDef hstatus;
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
    {
        hstatus = HAL_UART_Transmit(gHuart, (uint8_t *) ptr, len, HAL_MAX_DELAY);
        if (hstatus == HAL_OK)
            return len;
        else
            return EIO;
        // HAL_UART_Transmit_DMA(gHuart, (uint8_t *) ptr, len);
        // return len;
    } else
        return -1;
}

int _close(int fd)
{
    if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
        return 0;

    errno = EBADF;
    return -1;
}

int _lseek(int fd, int ptr, int dir)
{
    (void) fd;
    (void) ptr;
    (void) dir;

    errno = EBADF;
    return -1;
}

int _read(int fd, char *ptr, int len)
{
    HAL_StatusTypeDef hstatus;

    if (fd == STDIN_FILENO)
    {
        hstatus = HAL_UART_Receive(gHuart, (uint8_t *) ptr, 1, HAL_MAX_DELAY);
        if (hstatus == HAL_OK)
            return 1;
        else
            return EIO;
    }
    errno = EBADF;
    return -1;
}

int _fstat(int fd, struct stat *st)
{
    if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
    {
        st->st_mode = S_IFCHR;
        return 0;
    }

    errno = EBADF;
    return 0;
}

#endif //#if !defined(OS_USE_SEMIHOSTING)