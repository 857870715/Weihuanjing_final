#include "processcommand.h"
#include "mqtt.h"
#include "gpio.h"

uint8_t NBAcquisitionFlag = 0;

void GetDataAndSend(void)
{
    // char transmitBuffer1[100], transmitBuffer2[100], transmitBuffer3[100];
    float temperature1, humidity1;
    float temperature2, humidity2;
    float temperature3, humidity3;
    HAL_UART_Receive_DMA(&huart2, RxBuffer, sizeof(RxBuffer));
	HAL_GPIO_WritePin(GPIOB, I2C_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, NB_EN_Pin, GPIO_PIN_SET);
    while(NBAcquisitionFlag == 0) {
        HAL_Delay(100); // Small delay to prevent CPU hogging
    }
    NBAcquisitionFlag = 0;
    HAL_Delay(1000);
    MQTT_ConnectInit();
    SHT3X1_GetTempAndHumi(&temperature1, &humidity1, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
    SHT3X2_GetTempAndHumi(&temperature2, &humidity2, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
    SHT3X3_GetTempAndHumi(&temperature3, &humidity3, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
    HAL_Delay(1000);
    Sendmessage(temperature1, humidity1, temperature2, humidity2, temperature3, humidity3);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    HAL_Delay(200);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    CheckTimeSetRTC();
    HAL_Delay(1000);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(I2C_EN_GPIO_Port, I2C_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(NB_EN_GPIO_Port, NB_EN_Pin, GPIO_PIN_RESET);
}

void PWR_LED(void)
{
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, NB_EN_Pin, GPIO_PIN_SET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(NB_EN_GPIO_Port, NB_EN_Pin, GPIO_PIN_RESET);
}