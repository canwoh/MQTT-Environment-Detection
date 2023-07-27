#include"common_inc.h"
#include"OLED_Driver.h"
#include"net_connect.h"
#include"dht11.h"
void BSP_Init(void)
{
    OLED_Init();
    NetConnectInit();
    DHT11Init();
}

//-----------Testing-----------
uint8_t temp;
uint8_t hudm;
//-----------------------------

void MainEntryPoint(void)
{
    BSP_Init();
    for(;;)
    {
        DHT11ReadData(&temp,&hudm);
        printf("temp: %d\r\n",temp);
        printf("hudm: %d\r\n",hudm);
        printf("********************hello************************\r\n");
        HAL_Delay(1000);
    }
}