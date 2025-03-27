/*
 * mqtt.h
 *
 *  Created on: Mar 31, 2024
 *      Author: Andy
 */

#ifndef MQTT_H_
#define MQTT_H_

#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"

// #define MQTT_ID	"k0oz30rCDAx.SLM130|securemode=2,signmethod=hmacsha256,timestamp=1741606575867|"
// #define MQTT_USERNAME	"SLM130&k0oz30rCDAx"
// #define MQTT_PASSWD	"e941a214317aaa737bb9d8411d69059311a956bdeb3d5889151a437821a940d4"
#define MQTT_HOST	"iot-06z00buk99e09uc.mqtt.iothub.aliyuncs.com"
#define MQTT_PORT	1883
// #define MQTT_TOPIC_SEND	"/sys/k0oz3Dc4wL7/NBSLM130/thing/event/property/post"

// #define PRODUCT_KEY		"k0oz3Dc4wL7"
// #define DEVICE_NAME		"NBSLM130"
// #define DEVICE_SECRET	"18a7f1ef7e7cde255cec32319dcb728d"
// #define PRODUCT_NAME	"NBtest"

void MQTT_ConnectInit(void);
void Sendmessage(float temperature1, float humidity1,
    float temperature2, float humidity2,
    float temperature3, float humidity3);
void sendATCmd(const char* cmd, uint32_t delay_ms);

#endif /* HARDWARE_MQTT_H_ */
