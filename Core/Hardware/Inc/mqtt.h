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
#include "processCommand.h"

#define MQTT_HOST	"iot-06z00buk99e09uc.mqtt.iothub.aliyuncs.com"
#define MQTT_PORT	1883


void MQTT_ConnectInit(void);
void Sendmessage(float temperature1, float humidity1,
    float temperature2, float humidity2,
    float temperature3, float humidity3, uint8_t last_rssi);
void sendATCmd(const char* cmd, uint32_t delay_ms);
void CheckTimeSetRTC(void);
void CheckCSQ(void);

#endif /* HARDWARE_MQTT_H_ */
