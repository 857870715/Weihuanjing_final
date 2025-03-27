//=============================================================================
//	S E N S I R I O N	AG,	Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//=============================================================================
// Project	:	SHT3X3 Sample Code (V1.1)
// File	:	sht3X3.h (V1.1)
// Author	:	RFU
// Date	:	6-Mai-2015
// Controller:	STM32F100RB
// IDE	:	µVision V5.12.0.0
// Compiler	:	Armcc
// Brief	:	Sensor Layer: Definitions of commands and functions for sensor
//	access.
//=============================================================================

#ifndef SHT3X_3_H
#define SHT3X_3_H

//-- Includes -----------------------------------------------------------------
#include "I2c3_hal.h"
#include "I2c1_hal.h"
#include "sht3x_1.h"
#include "system.h" 

//-- Enumerations -------------------------------------------------------------
// Sensor Commands

//=============================================================================
// Initializes the I2c3 bus for communication with the sensor.
 

//-----------------------------------------------------------------------------
// input: I2c3Address	I2c3 address, 0x44 ADDR pin low / 0x45 ADDR pin high
//-----------------------------------------------------------------------------
void SHT3X3_Init(u8t I2c3Address);


//=============================================================================
// Sets the I2c3 address.
//-----------------------------------------------------------------------------
// input: I2c3Address	I2c3 address, 0x44 ADDR pin low / 0x45 ADDR pin high
//-----------------------------------------------------------------------------
void SHT3X3_SetI2c3Adr(u8t I2c3Address);


//=============================================================================
// Reads the serial number from sensor.
//-----------------------------------------------------------------------------
// input: serialNumber	pointer to serialNumber
//
// return: error:	ACK_ERROR	= no acknowledgment from sensor
//	CHECKSUM_ERROR = checksum mismatch
//	TIMEOUT_ERROR	= timeout
//	NO_ERROR	= no error
//-----------------------------------------------------------------------------
etError SHT3X3_ReadSerialNumber(u32t* serialNumber);


//=============================================================================
// Reads the status register from the sensor.
//-----------------------------------------------------------------------------
// input: status	pointer to status
//
// return: error:	ACK_ERROR	= no acknowledgment from sensor
//	CHECKSUM_ERROR = checksum mismatch
//	TIMEOUT_ERROR	= timeout
//	NO_ERROR	= no error
//-----------------------------------------------------------------------------
etError SHT3X3_ReadStatus(u16t* status);


//=============================================================================
// Clears all alert flags in status register from sensor.
//-----------------------------------------------------------------------------
// return: error:	ACK_ERROR	= no acknowledgment from sensor
//	CHECKSUM_ERROR = checksum mismatch
//	TIMEOUT_ERROR	= timeout
//	NO_ERROR	= no error
//-----------------------------------------------------------------------------
etError SHT3X3_ClearAllAlertFlags(void);


//=============================================================================
// Gets the temperature [°C] and the relative humidity [%RH] from the sensor.
//-----------------------------------------------------------------------------
// input: temperature	pointer to temperature
//	humiditiy	pointer to humidity
//	repeatability repeatability for the measurement [low, medium, high]
//	mode	command mode [clock stretching, polling]
//	timeout	timeout in milliseconds
//
// return: error:	ACK_ERROR	= no acknowledgment from sensor
//	CHECKSUM_ERROR = checksum mismatch
 

//	TIMEOUT_ERROR	= timeout
//	PARM_ERROR	= parameter out of range
//	NO_ERROR	= no error
//-----------------------------------------------------------------------------
etError SHT3X3_GetTempAndHumi(ft* temperature, ft* humiditiy,
etRepeatability repeatability, etMode mode, u8t timeout);


//=============================================================================
// Gets the temperature [°C] and the relative humidity [%RH] from the sensor.
// This function uses the I2c3 clock stretching for waiting until measurement is
// ready.
//-----------------------------------------------------------------------------
// input: temperature	pointer to temperature
//	humiditiy	pointer to humidity
//	repeatability repeatability for the measurement [low, medium, high]
//	timeout	clock stretching timeout in milliseconds
//
// return: error:	ACK_ERROR	= no acknowledgment from sensor
//	CHECKSUM_ERROR = checksum mismatch
//	TIMEOUT_ERROR	= timeout
//	PARM_ERROR	= parameter out of range
//	NO_ERROR	= no error
//-----------------------------------------------------------------------------
etError SHT3X3_GetTempAndHumiClkStretch(ft* temperature, ft* humiditiy,
etRepeatability repeatability, u8t timeout);


//=============================================================================
// Gets the temperature [°C] and the relative humidity [%RH] from the sensor.
// This function polls every 1ms until measurement is ready.
//-----------------------------------------------------------------------------
// input: temperature	pointer to temperature
//	humiditiy	pointer to humidity
//	repeatability repeatability for the measurement [low, medium, high]
//	timeout	polling timeout in milliseconds
//
// return: error:	ACK_ERROR	= no acknowledgment from sensor
//	CHECKSUM_ERROR = checksum mismatch
//	TIMEOUT_ERROR	= timeout
//	PARM_ERROR	= parameter out of range
//	NO_ERROR	= no error
//-----------------------------------------------------------------------------
etError SHT3X3_GetTempAndHumiPolling(ft* temperature, ft* humiditiy,
etRepeatability repeatability, u8t timeout);


//=============================================================================
// Starts periodic measurement.
//-----------------------------------------------------------------------------
// input: repeatability repeatability for the measurement [low, medium, high]
//	frequency	measurement frequency [0.5, 1, 2, 4, 10] Hz
//
// return: error:	ACK_ERROR	= no acknowledgment from sensor
//	CHECKSUM_ERROR = checksum mismatch
//	TIMEOUT_ERROR	= timeout
//	PARM_ERROR	= parameter out of range
//	NO_ERROR	= no error
//-----------------------------------------------------------------------------
 

etError SHT3X3_StartPeriodicMeasurment(etRepeatability repeatability,
etFrequency frequency);


//=============================================================================
// Reads last measurement from the sensor buffer
//-----------------------------------------------------------------------------
// input: temperature	pointer to temperature
//	humidity	pointer to humidity
//
// return: error:	ACK_ERROR	= no acknowledgment from sensor
//	CHECKSUM_ERROR = checksum mismatch
//	TIMEOUT_ERROR	= timeout
//	NO_ERROR	= no error
//-----------------------------------------------------------------------------
etError SHT3X3_ReadMeasurementBuffer(ft* temperature, ft* humidity);


//=============================================================================
// Enables the heater on sensor
//-----------------------------------------------------------------------------
// return: error:	ACK_ERROR	= no acknowledgment from sensor
//	CHECKSUM_ERROR = checksum mismatch
//	TIMEOUT_ERROR	= timeout
//	NO_ERROR	= no error
//-----------------------------------------------------------------------------
etError SHT3X3_EnableHeater(void);


//=============================================================================
// Disables the heater on sensor
//-----------------------------------------------------------------------------
// return: error:	ACK_ERROR	= no acknowledgment from sensor
//	CHECKSUM_ERROR = checksum mismatch
//	TIMEOUT_ERROR	= timeout
//	NO_ERROR	= no error
//-----------------------------------------------------------------------------
etError SHT3X3_DisableHeater(void);

//=============================================================================
//
//-----------------------------------------------------------------------------
etError SHT3X3_SetAlertLimits(ft humidityHighSet,	ft temperatureHighSet,
ft humidityHighClear, ft temperatureHighClear, ft humidityLowClear,	ft temperatureLowClear, ft humidityLowSet,	ft temperatureLowSet);

//=============================================================================
//
//-----------------------------------------------------------------------------
etError SHT3X3_GetAlertLimits(ft* humidityHighSet,	ft* temperatureHighSet,
ft* humidityHighClear, ft* temperatureHighClear, ft* humidityLowClear,	ft* temperatureLowClear, ft* humidityLowSet,	ft* temperatureLowSet);

//=============================================================================
// Returns the state of the Alert-Pin.
//-----------------------------------------------------------------------------
// return:	true:	Alert-Pin is high
//	false: Alter-Pin is low
//-----------------------------------------------------------------------------
bt SHT3X3_ReadAlert(void);
 


//=============================================================================
// Calls the soft reset mechanism that forces the sensor into a well-defined
// state without removing the power supply.
//-----------------------------------------------------------------------------
// return: error:	ACK_ERROR	= no acknowledgment from sensor
//	CHECKSUM_ERROR = checksum mismatch
//	TIMEOUT_ERROR	= timeout
//	NO_ERROR	= no error
//-----------------------------------------------------------------------------
etError SHT3X3_SoftReset(void);


//=============================================================================
// Resets the sensor by pulling down the reset pin.
//-----------------------------------------------------------------------------
void SHT3X3_HardReset(void);


#endif
