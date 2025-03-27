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
	HAL_GPIO_WritePin(GPIOB, NB_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(5000);
	SHT3X1_GetTempAndHumi(&temperature1, &humidity1, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
	SHT3X2_GetTempAndHumi(&temperature2, &humidity2, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
	SHT3X3_GetTempAndHumi(&temperature3, &humidity3, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
	HAL_Delay(5000);
	MQTT_ConnectInit();
	Sendmessage(temperature1, humidity1, temperature2, humidity2, temperature3, humidity3);
	HAL_Delay(2000);
	HAL_GPIO_WritePin(GPIOB, I2C_EN_Pin, GPIO_PIN_RESET);
  	HAL_GPIO_WritePin(GPIOB, NB_EN_Pin, GPIO_PIN_RESET);
}

// void processTem1(void)
// {
// 	SHT3X1_GetTempAndHumi(&temperature1, &humidity1, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
// 	sprintf(transmitBuffer1, "t1=%.1f\r\nh1=%.1f%%\r\n", (float)temperature1,(float)humidity1);
// 	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)transmitBuffer1, strlen(transmitBuffer1));
// 	HAL_Delay(10);
// }

// void processTem2(void)
// {
// 	SHT3X2_GetTempAndHumi(&temperature2, &humidity2, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
// 	sprintf(transmitBuffer2, "t2=%.1f\r\nh2=%.1f%%\r\n", (float)temperature2,(float)humidity2);
// 	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)transmitBuffer2, strlen(transmitBuffer2));
// 	HAL_Delay(10);
// }

// void processTem3(void)
// {
// 	SHT3X3_GetTempAndHumi(&temperature3, &humidity3, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
// 	sprintf(transmitBuffer3, "t3=%.1f\r\nh3=%.1f%%\r\n", (float)temperature3,(float)humidity3);
// 	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)transmitBuffer3, strlen(transmitBuffer3));
// 	HAL_Delay(10);
// }
