#ifndef FLASH_STORAGE_H_
#define FLASH_STORAGE_H_

#include "main.h"
#include "processCommand.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_flash_ex.h"
#include <string.h>
#include <time.h>

/* FLASH 总区间 Base + Size (256 KB) */
#define FLASH_BASE_ADDR    ((uint32_t)0x08000000U)
#define FLASH_TOTAL_SIZE   ((uint32_t)0x00040000U)

/* 每页大小 2 KB */
#define FLASH_PAGE_SIZE    ((uint32_t)0x800U)

/* 保留最后一页给设备识别码 */
#define FLASH_USER_END     (FLASH_BASE_ADDR + FLASH_TOTAL_SIZE - FLASH_PAGE_SIZE)

/* 预留 32 KB 给固件（根据你实际固件大小可调整）并对齐到下一个页边界 */
#define FLASH_USER_START   (FLASH_BASE_ADDR + 0x08000U)   // 即 0x08008000

#define FLASH_BANK         FLASH_BANK_1

/* 每条记录 16 B = 4B 时间 + 6×2B 传感器值 */
#define RECORD_SIZE_BYTES  (16U)

#pragma pack(push,1)
typedef struct {
    uint32_t timestamp_min;  // 自 1970-01-01 起的分钟戳
    int16_t  temp1;          // 温度×10
    int16_t  temp2;
    int16_t  temp3;
    int16_t  hum1;           // 湿度×10
    int16_t  hum2;
    int16_t  hum3;
} DataRecord_t;
#pragma pack(pop)

extern RTC_HandleTypeDef hrtc;

uint32_t             GetTimestampMin(void);
uint32_t             FindNextFlashAddr(void);
HAL_StatusTypeDef    SaveDataToFlash(const void *rec, uint32_t size, uint32_t writeAddr);
HAL_StatusTypeDef    ErasePageAtAddr(uint32_t writeAddr);

#endif // !FLASH_STORAGE_H_
