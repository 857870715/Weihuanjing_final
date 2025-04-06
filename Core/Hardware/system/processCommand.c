#include "processcommand.h"
#include "mqtt.h"
#include "gpio.h"

// char transmitBuffer1[100], transmitBuffer2[100], transmitBuffer3[100];
float temperature1, humidity1;
float temperature2, humidity2;
float temperature3, humidity3;

void GetDataAndSend(void)
{
	HAL_GPIO_WritePin(GPIOB, I2C_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, NB_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(5000);
	SHT3X1_GetTempAndHumi(&temperature1, &humidity1, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
	SHT3X2_GetTempAndHumi(&temperature2, &humidity2, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
	SHT3X3_GetTempAndHumi(&temperature3, &humidity3, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
	HAL_Delay(5000);
	MQTT_ConnectInit();
	Sendmessage(temperature1, humidity1, temperature2, humidity2, temperature3, humidity3);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    CheckTimeSetRTC();
	HAL_Delay(2000);
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