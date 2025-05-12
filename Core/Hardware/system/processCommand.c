#include "processcommand.h"
#include "mqtt.h"
#include "gpio.h"

uint8_t NBAcquisitionFlag = 0;
uint8_t  CSQAcquisitionFlag = 0;
uint8_t last_rssi = 0;

void GetDataAndSend(void)
{
    // char transmitBuffer1[100], transmitBuffer2[100], transmitBuffer3[100];
    float temperature1, humidity1;
    float temperature2, humidity2;
    float temperature3, humidity3;
    HAL_UART_Receive_DMA(&huart2, RxBuffer, sizeof(RxBuffer));
	HAL_GPIO_WritePin(GPIOB, I2C_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, NB_EN_Pin, GPIO_PIN_SET);
    uint32_t timeout_counter = 0;
    uint32_t timeout_limit = 600; // 60 seconds (30,000ms / 100ms)
    
    while(NBAcquisitionFlag == 0) {
        HAL_Delay(100); // Small delay to prevent CPU hogging
        timeout_counter++;
        
        if(timeout_counter >= timeout_limit) {
            timeout_counter = 0; // Reset the counter
            // If 30 seconds have passed and still no signal, enter SHUTDOWN mode
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(I2C_EN_GPIO_Port, I2C_EN_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(NB_EN_GPIO_Port, NB_EN_Pin, GPIO_PIN_RESET);
            HAL_PWREx_EnterSHUTDOWNMode();
        }
    }
    NBAcquisitionFlag = 0;

    HAL_Delay(1000);
    MQTT_ConnectInit();

    SHT3X1_GetTempAndHumi(&temperature1, &humidity1, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
    SHT3X2_GetTempAndHumi(&temperature2, &humidity2, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
    SHT3X3_GetTempAndHumi(&temperature3, &humidity3, REPEATAB_HIGH, MODE_CLKSTRETCH, 50);
    CheckCSQ();

    HAL_Delay(1000);
    Sendmessage(temperature1, humidity1, temperature2, humidity2, temperature3, humidity3, last_rssi);

    HAL_Delay(1000);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    HAL_Delay(200);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

    CheckTimeSetRTC();
    HAL_Delay(1000);

    // —— ① 构造一条 16B 的记录 —— 
    DataRecord_t rec;
    rec.timestamp_min = GetTimestampMin();
    rec.temp1 = (int16_t)(temperature1 * 10);
    rec.temp2 = (int16_t)(temperature2 * 10);
    rec.temp3 = (int16_t)(temperature3 * 10);
    rec.hum1  = (int16_t)(humidity1    * 10);
    rec.hum2  = (int16_t)(humidity2    * 10);
    rec.hum3  = (int16_t)(humidity3    * 10);

    // —— ② 找下一个可写地址 —— 
    uint32_t writeAddr = FindNextFlashAddr();

    // —— ③ 如果空间足够，就写入 —— 
    if (writeAddr + RECORD_SIZE_BYTES <= FLASH_USER_END) {
        // 正常追加：SaveDataToFlash 内部会在新页头擦除该页
        SaveDataToFlash(&rec, RECORD_SIZE_BYTES, writeAddr);
    } else {
        // 满区，从头循环：写地址指向起始
        writeAddr = FLASH_USER_START;
        // 先擦除这个起始页
        if (ErasePageAtAddr(writeAddr) == HAL_OK) {
            // 然后写入
            SaveDataToFlash(&rec, RECORD_SIZE_BYTES, writeAddr);
        } else {
            // 擦除失败，做错误处理
            // HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        }
    }

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