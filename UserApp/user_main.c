#include "common_inc.h"
#include "OLED_Driver.h"
#include "net_connect.h"
#include "dht11.h"
#include"led.h"
#include"string.h"

//-----------DATA-----------
uint8_t temp;
uint8_t humi;
extern TIM_HandleTypeDef htim2;
extern char *AT_MODE_STR;
extern const uint16_t MODE_SIZE;
extern UART_HandleTypeDef huart2;
extern uint8_t receive_buf[256];
uint8_t TASK_OLED = 0;
uint8_t TASK_DHT11 = 0;
uint8_t TASK_LED = 0;
uint8_t TASK_SEND_MSG = 0;
//-----------------------------

//--------------------Initialization------------------------
void BSP_Init(void)
{
   // OLED_Init();
    NetConnectInit();
    DHT11Init();
    HAL_TIM_Base_Start_IT(&htim2);
}
//---------------------------------------------------------


//-----------------------------Task Control------------------------

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint16_t timer = 0;
    if (htim->Instance == htim2.Instance)
    {
        timer++;
        if (timer % 50 == 0)
        {
            TASK_OLED = 1;
            TASK_DHT11 = 1;
            TASK_LED = 1;
            //--------------修改模拟温湿度上传-------------
            temp++;
            humi++;
            //----------------------------------------
        }
        if (timer > 200)
        {
            TASK_SEND_MSG = 1;
            timer = 0;
            //--------------模拟温湿度上传-------------
            temp = 0;
            humi = 0;
            //----------------------------------------
        }
    }
}

//----------------------------------------------------------------

void MainEntryPoint(void)
{
    //上电延迟
    HAL_Delay(3000);
    memset(receive_buf, 0X00, sizeof(receive_buf));
    BSP_Init();
    for (;;)
    {
        ESP8266ReceiveMsg();
        if(TASK_DHT11==1)
        {
            //DHT11ReadData(&temp, &humi);
            ESP8266SendMsg();
            TASK_DHT11 = 0;
        }
        if(TASK_LED==1)
        {
            SetLedState();
            TASK_LED=0;
        }
        if(TASK_OLED==1)
        {
            OLED_ShowString(1,1,"Hello world!");
            OLED_ShowString(2,1,"Humi: ");
            OLED_ShowNum(2,6,humi,2);
            OLED_ShowString(3,1,"Temp: ");
            OLED_ShowNum(3,6,temp,2);
            TASK_OLED=0;
        }
    }
}