/*
 * mqtt.c
 *
 *  Created on: Mar 31, 2024
 *      Author: Andy
 */
#include "mqtt.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>
#include <string.h>

// 预留配置数据的 Flash 地址（请根据你的芯片内存布局确认）
#define CONFIG_FLASH_ADDRESS  0x0803F800

// 定义设备配置结构体，注意字段大小必须与生成配置文件时保持一致
typedef struct {
    char device_name[32];    // 例如 "NBSLM13001"
    char device_secret[64];  // 例如 "18a7f1ef7e7cde255cec32319dcb728d"
    char product_name[32];   // 例如 "NBtest"
    char product_key[32];    // 例如 "k0oz3Dc4wL7"
} DeviceConfig;

// 全局变量用于保存从 Flash 读取的设备配置数据
static DeviceConfig g_deviceConfig;

// 全局 MQTT 主题字符串（构造格式：/sys/<PRODUCT_KEY>/<DEVICE_NAME>/thing/event/property/post）
static char MQTT_TOPIC_SEND[128] = "";

extern char outStr[64];
extern uint8_t RxBuffer[40];
extern uint8_t CSQBuffer[20];
extern DMA_HandleTypeDef hdma_usart2_rx;
extern RTC_HandleTypeDef hrtc;
extern uint8_t  CSQAcquisitionFlag;
extern uint8_t last_rssi;
/**
 * @brief 从 Flash 中读取设备配置数据
 */
static void LoadDeviceConfig(void)
{
    // 直接将 Flash 中的数据复制到 g_deviceConfig 中
    memcpy(&g_deviceConfig, (void*)CONFIG_FLASH_ADDRESS, sizeof(DeviceConfig));
    
    // 调试输出，确认读取结果
    // printf("Loaded config from Flash:\r\n");
    // printf("DEVICE_NAME: %s\r\n", g_deviceConfig.device_name);
    // printf("DEVICE_SECRET: %s\r\n", g_deviceConfig.device_secret);
    // printf("PRODUCT_NAME: %s\r\n", g_deviceConfig.product_name);
    // printf("PRODUCT_KEY: %s\r\n", g_deviceConfig.product_key);
}

/**
 * @brief 根据从 Flash 中读取的设备配置构造 MQTT 主题字符串
 */
static void UpdateMQTTTopic(void)
{
    // MQTT 主题格式：/sys/<PRODUCT_KEY>/<DEVICE_NAME>/thing/event/property/post
    snprintf(MQTT_TOPIC_SEND, sizeof(MQTT_TOPIC_SEND),
             "/sys/%s/%s/thing/event/property/post",
             g_deviceConfig.product_key,
             g_deviceConfig.device_name);
}

void sendATCmd(const char* cmd, uint32_t delay_ms)
{
    // 发送 AT 指令，并自动换行
    char buffer[256];
    int len = sprintf(buffer, "%s\r\n", cmd);
    HAL_UART_Transmit_DMA(&huart2, (uint8_t*)buffer, len);
    HAL_Delay(delay_ms);
}

void MQTT_ConnectInit(void)
{
    char cmdBuffer[256];

    // 1. 先从 Flash 中加载设备配置
    LoadDeviceConfig();
    // 2. 根据读取的配置更新 MQTT 主题字符串
    UpdateMQTTTopic();

    //配置保活时间
    sendATCmd("AT+ECMTCFG=\"keepalive\",0,1200", 1000);

    // 发送云平台配置命令
    sendATCmd("AT+ECMTCFG=\"cloud\",0,2,1", 1000);

    // 构造认证命令，使用从 Flash 中读取到的设备参数
    snprintf(cmdBuffer, sizeof(cmdBuffer),
             "AT+ECMTCFG=\"aliauth\",0,\"%s\",\"%s\",\"%s\"",
             g_deviceConfig.product_key,
             g_deviceConfig.device_name,
             g_deviceConfig.device_secret);
    sendATCmd(cmdBuffer, 2000);

    // 构造打开 MQTT 连接命令（MQTT_HOST 仍使用宏定义）
    snprintf(cmdBuffer, sizeof(cmdBuffer),
             "AT+ECMTOPEN=0,\"%s\",1883", MQTT_HOST);
    sendATCmd(cmdBuffer, 10000);

    // 构造建立连接命令，使用产品名称
    snprintf(cmdBuffer, sizeof(cmdBuffer),
             "AT+ECMTCONN=0,\"%s\"", g_deviceConfig.product_name);
    sendATCmd(cmdBuffer, 3000);
}

void Sendmessage(float temperature1, float humidity1,
                 float temperature2, float humidity2,
                 float temperature3, float humidity3, uint8_t last_rssi) 
{
    char payload[256];
    // char cmdBuffer_Sendmessage[256];
    snprintf(payload, sizeof(payload),
             "{\"params\":{"
             "\"temperature1\":%.1f,\"humidity1\":%.1f,"
             "\"temperature2\":%.1f,\"humidity2\":%.1f,"
             "\"temperature3\":%.1f,\"humidity3\":%.1f,"
             "\"rssi\":%d"
             "}}",
             temperature1, humidity1,
             temperature2, humidity2,
             temperature3, humidity3, last_rssi);

    printf("AT+ECMTPUB=0,0,0,0,\"%s\",%s\r\n", MQTT_TOPIC_SEND, payload);
    HAL_Delay(100);
}
void CheckTimeSetRTC(void)
{
    char cmdBuffer_RTC[20];
    HAL_UART_Receive_DMA(&huart2, RxBuffer, sizeof(RxBuffer));
    sprintf(cmdBuffer_RTC, "AT+CCLK?");
    sendATCmd(cmdBuffer_RTC, 100);
}

void CheckCSQ(void)
{
    char cmdBuffer_CSQ[20];
    HAL_UART_Receive_DMA(&huart2, CSQBuffer, sizeof(CSQBuffer));
    sprintf(cmdBuffer_CSQ, "AT+CSQ");
    sendATCmd(cmdBuffer_CSQ, 100);
}

/**
  * @brief  UART接收完成回调函数
  * @param  huart: 串口句柄
  * @note   当UART DMA接收到指定长度的数据（这里是100字节）后就会调用此回调
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        char *pLine = (char*)RxBuffer;  // 将接收缓冲转成字符串指针
        char *pIP = strstr(pLine, "+IP:");
        if (pIP != NULL)
        {
            NBAcquisitionFlag = 1; // 设置标志位，表示接收到了NB数据
            return; // 直接返回，避免继续处理
        }
    
        // 在RxBuffer中查找 +CCLK
        char *pClk = strstr(pLine, "+CCLK: ");
        if (pClk != NULL)
        {
            int year, month, date;
            int hour, minute, second;
            int timezone; // 如果需要可以使用

            // 解析: +CCLK: "YYYY/MM/DD,hh:mm:ss+ZZ"
            if (sscanf(pClk, "+CCLK: \"%d/%d/%d,%d:%d:%d+%d\"",
                    &year, &month, &date,
                    &hour, &minute, &second,
                    &timezone) == 7)
            {
                // 1) 将解析出的时间写入RTC
                RTC_TimeTypeDef sTime = {0};
                RTC_DateTypeDef sDate = {0};

                sDate.Year  = (uint8_t)(year - 2000);
                sDate.Month = (uint8_t)month;
                sDate.Date  = (uint8_t)date;

                sTime.Hours   = (uint8_t)hour;
                sTime.Minutes = (uint8_t)minute;
                sTime.Seconds = (uint8_t)second;
                sTime.TimeFormat = RTC_HOURFORMAT12_AM; 

                HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
                HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

                // 2) 读取刚刚写入的RTC时间，并通过串口2发送出去
                RTC_TimeTypeDef sTimeRead = {0};
                RTC_DateTypeDef sDateRead = {0};

                // 读取RTC，先读time再读date（HAL库要求）
                HAL_RTC_GetTime(&hrtc, &sTimeRead, RTC_FORMAT_BIN);
                HAL_RTC_GetDate(&hrtc, &sDateRead, RTC_FORMAT_BIN);

                // 拼接输出字符串
                
                // 这里用 20%02d 处理两位数的Year，假设Year是在2000~2099
                snprintf(outStr, sizeof(outStr),
                        "Current RTC: 20%02d/%02d/%02d %02d:%02d:%02d\r\n",
                        sDateRead.Year,
                        sDateRead.Month,
                        sDateRead.Date,
                        sTimeRead.Hours,
                        sTimeRead.Minutes,
                        sTimeRead.Seconds);

                // 发送到串口2 (DMA方式)
                HAL_UART_Transmit_DMA(&huart2, (uint8_t*)outStr, strlen(outStr));
            }
            return; // 直接返回，避免继续处理
        }

        char *pCSQ = strstr((char*)CSQBuffer, "+CSQ: ");
        if (pCSQ) 
        {
            int rssi, ber;
            if (sscanf(pCSQ, "+CSQ: %d,%d", &rssi, &ber) == 2)
            {
                last_rssi = rssi;
            }
        }
        
        // 如果要继续接收后续NB数据，必须再启动一次DMA接收
        HAL_UART_Receive_DMA(&huart2, RxBuffer, sizeof(RxBuffer));
    }
}