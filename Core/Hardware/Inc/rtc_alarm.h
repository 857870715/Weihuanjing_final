/*
 * rtc_alarm.h
 *
 *  Created on: Mar 4, 2025
 *      Author: Andy
 */

#ifndef USER_INC_RTC_ALARM_H_
#define USER_INC_RTC_ALARM_H_

#include "main.h"
#include "usart.h"

void Set_Next_Alarm(RTC_HandleTypeDef *hrtc, uint32_t delaySeconds);

#endif /* USER_INC_RTC_ALARM_H_ */
