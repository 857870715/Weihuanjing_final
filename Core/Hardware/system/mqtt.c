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
    printf("%s\r\n", cmd);
    HAL_Delay(delay_ms);
}

void MQTT_ConnectInit(void)
{
    char cmdBuffer[256];

    // 1. 先从 Flash 中加载设备配置
    LoadDeviceConfig();
    // 2. 根据读取的配置更新 MQTT 主题字符串
    UpdateMQTTTopic();

    // 发送云平台配置命令
    sendATCmd("AT+ECMTCFG=\"cloud\",0,2,1", 2000);

    // 构造认证命令，使用从 Flash 中读取到的设备参数
    snprintf(cmdBuffer, sizeof(cmdBuffer),
             "AT+ECMTCFG=\"aliauth\",0,\"%s\",\"%s\",\"%s\"",
             g_deviceConfig.product_key,
             g_deviceConfig.device_name,
             g_deviceConfig.device_secret);
    sendATCmd(cmdBuffer, 3000);

    // 构造打开 MQTT 连接命令（MQTT_HOST 仍使用宏定义）
    snprintf(cmdBuffer, sizeof(cmdBuffer),
             "AT+ECMTOPEN=0,\"%s\",1883", MQTT_HOST);
    sendATCmd(cmdBuffer, 4000);

    // 构造建立连接命令，使用产品名称
    snprintf(cmdBuffer, sizeof(cmdBuffer),
             "AT+ECMTCONN=0,\"%s\"", g_deviceConfig.product_name);
    sendATCmd(cmdBuffer, 1000);
}

void Sendmessage(float temperature1, float humidity1,
                 float temperature2, float humidity2,
                 float temperature3, float humidity3) 
{
    char payload[256];
    snprintf(payload, sizeof(payload),
             "{\"params\":{"
             "\"temperature1\":%.1f,\"humidity1\":%.1f,"
             "\"temperature2\":%.1f,\"humidity2\":%.1f,"
             "\"temperature3\":%.1f,\"humidity3\":%.1f"
             "}}",
             temperature1, humidity1,
             temperature2, humidity2,
             temperature3, humidity3);

    // 使用构造好的 MQTT_TOPIC_SEND 发送 AT 指令
    printf("AT+ECMTPUB=0,0,0,0,\"%s\",%s\r\n", MQTT_TOPIC_SEND, payload);
    HAL_Delay(100);
}
