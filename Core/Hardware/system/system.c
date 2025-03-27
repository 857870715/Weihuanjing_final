//-- Includes -----------------------------------------------------------------
#include "system.h"
//#include "delay.h"
#include "stm32l4xx_hal.h"
////-----------------------------------------------------------------------------
//void SystemInit(void)
//{
//    // no initialization required
//}

//-----------------------------------------------------------------------------
void DelayMicroSeconds(u32t us)	/* -- adapt this delay for your uC -- */
{
//    uint32_t count = us * 72;  // 计算需要循环的次数
//
//    for (uint32_t i = 0; i < count; i++)
//    {
//        // 空操作，消耗一定的时间
//        //__asm("nop");
//    }
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while (delay--)
	{
		;
	}
    
}


