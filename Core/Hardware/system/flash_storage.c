#include "flash_storage.h"

/* ———— Helper: 从 RTC 读取当前 Unix 时间，并返回分钟级戳 ———— */
uint32_t GetTimestampMin(void)
{
    RTC_DateTypeDef sDate;
    RTC_TimeTypeDef sTime;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    struct tm tm;
    tm.tm_year = sDate.Year + 100;      // RTC.Year 从 0 对应 2000年
    tm.tm_mon  = sDate.Month - 1;       // tm_mon: 0-11
    tm.tm_mday = sDate.Date;
    tm.tm_hour = sTime.Hours;
    tm.tm_min  = sTime.Minutes;
    tm.tm_sec  = 0;                     // 精确到分
    tm.tm_isdst = 0;

    time_t t = mktime(&tm);
    return (uint32_t)(t / 60);
}

/* ———— 扫描用户区，找到第一个 FF 空白记录 起始地址 ———— */
uint32_t FindNextFlashAddr(void)
{
    uint32_t addr = FLASH_USER_START;
    while (addr + RECORD_SIZE_BYTES <= FLASH_USER_END) {
        if (*((uint32_t*)addr) == 0xFFFFFFFFU) {
            break;
        }
        addr += RECORD_SIZE_BYTES;
    }
    return addr;
}

/* 计算页号：0 对应 0x0800_0000–0x0800_07FF */
uint32_t AddrToPage(uint32_t addr) {
    return (addr - FLASH_BASE_ADDR) / FLASH_PAGE_SIZE;
}

/* 擦除一页 */
HAL_StatusTypeDef EraseFlashPage(uint32_t pageAddress)
{
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t pageError;

    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.Banks     = FLASH_BANK;
    eraseInit.Page      = AddrToPage(pageAddress);
    eraseInit.NbPages   = 1;

    return HAL_FLASHEx_Erase(&eraseInit, &pageError);
}

/* 将一条记录以 8 字节 (double‑word) 为单元写入 */
HAL_StatusTypeDef SaveDataToFlash(const void *rec, uint32_t size, uint32_t writeAddr)
{
    HAL_StatusTypeDef status = HAL_OK;
    const uint8_t *p = (const uint8_t*)rec;

    HAL_FLASH_Unlock();
    /* 每 8 字节写一次 */
    for (uint32_t offset = 0; offset < size; offset += 8) {
        uint64_t dw = 0;
        /* 把接下来的最多 8 字节打包到一个 uint64_t */
        memcpy(&dw, p + offset, (size - offset >= 8) ? 8 : (size - offset));
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, writeAddr + offset, dw);
        if (status != HAL_OK) break;
    }
    HAL_FLASH_Lock();
    return status;
}

/* 仅擦除 writeAddr 所在的那一页 */
HAL_StatusTypeDef ErasePageAtAddr(uint32_t writeAddr)
{
    uint32_t page = AddrToPage(writeAddr);
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t pageError;

    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.Banks     = FLASH_BANK;
    eraseInit.Page      = page;
    eraseInit.NbPages   = 1;

    HAL_FLASH_Unlock();
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&eraseInit, &pageError);
    HAL_FLASH_Lock();
    return status;
}