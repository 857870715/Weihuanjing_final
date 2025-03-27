/*
 * processCommand.h
 *
 *  Created on: Feb 28, 2024
 *      Author: Andy
 */

#ifndef PROCESSCOMMAND_H_
#define PROCESSCOMMAND_H_

#include "main.h"
#include "i2c1_hal.h"
#include "i2c2_hal.h"
#include "i2c3_hal.h"
#include "sht3x_1.h"
#include "sht3x_2.h"
#include "sht3x_3.h"
#include "stdio.h"
#include <string.h>

#include "dwt_delay.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"


// void processTem1(void);

// void processTem2(void);

// void processTem3(void);

void GetDataAndSend(void);

#endif /* HARDWARE_PROCESSCOMMAND_H_ */
