/*
 * rtc_alarm.c
 *
 *  Created on: Mar 4, 2025
 *      Author: Andy
 */
#include "rtc_alarm.h"

void Set_Next_Alarm(RTC_HandleTypeDef *hrtc, uint32_t delaySeconds)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    RTC_AlarmTypeDef sAlarm = {0};

    /* 获取当前时间和日期 */
    if (HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }

    /* 将当前时间转换为总秒数 */
    uint32_t currentSeconds = sTime.Hours * 3600 + sTime.Minutes * 60 + sTime.Seconds;
    uint32_t newTimeSeconds = currentSeconds + delaySeconds;

    /* 计算新的时、分、秒 */
    uint8_t newHour = (newTimeSeconds / 3600) % 24;
    uint8_t newMin  = (newTimeSeconds % 3600) / 60;
    uint8_t newSec  = newTimeSeconds % 60;

    /* 配置新的 Alarm A */
    sAlarm.AlarmTime.Hours = newHour;
    sAlarm.AlarmTime.Minutes = newMin;
    sAlarm.AlarmTime.Seconds = newSec;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;

    /* 为了精确匹配延时后的时间，不使用时间掩码 */
    sAlarm.AlarmMask = 0;  // 完全匹配
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay = sDate.Date;  // 测试阶段不处理跨日问题
    sAlarm.Alarm = RTC_ALARM_A;

    if (HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
}



