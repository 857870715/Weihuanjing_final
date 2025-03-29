//=============================================================================
//	S E N S I R I O N	AG,	Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//=============================================================================
// Project	:	SHT3x Sample Code (V1.1)
// File	:	I2c3_hal.c (V1.1)
// Author	:	RFU
// Date	:	6-Mai-2015
// Controller:	STM32F100RB
// IDE	:	µVision V5.12.0.0
// Compiler	:	Armcc
// Brief	:	I2c3 hardware abstraction layer
//=============================================================================

//-- Includes -----------------------------------------------------------------
#include "gpio.h"
#include "I2c3_hal.h"

/* 端口声明 */
#define Sht30_I2c3_Sda_GPIO GPIOB
#define Sht30_I2c3_Sda_ClockCmd RCC_APB2Periph_GPIOB
#define Sht30_I2c3_Sda_PIN GPIO_PIN_3
#define Sht30_I2c3_Scl_GPIO GPIOA
#define Sht30_I2c3_Scl_ClockCmd RCC_APB2Periph_GPIOA
#define Sht30_I2c3_Scl_PIN GPIO_PIN_15


#define Sht30_I2c3_Scl_SET HAL_GPIO_WritePin(Sht30_I2c3_Scl_GPIO,Sht30_I2c3_Scl_PIN,GPIO_PIN_SET)
#define Sht30_I2c3_Scl_CLR HAL_GPIO_WritePin(Sht30_I2c3_Scl_GPIO,Sht30_I2c3_Scl_PIN,GPIO_PIN_RESET)
#define Sht30_I2c3_Scl_READ HAL_GPIO_ReadPin(Sht30_I2c3_Scl_GPIO,Sht30_I2c3_Scl_PIN)
#define Sht30_I2c3_Sda_SET HAL_GPIO_WritePin(Sht30_I2c3_Sda_GPIO,Sht30_I2c3_Sda_PIN,GPIO_PIN_SET)
#define Sht30_I2c3_Sda_CLR HAL_GPIO_WritePin(Sht30_I2c3_Sda_GPIO,Sht30_I2c3_Sda_PIN,GPIO_PIN_RESET)
#define Sht30_I2c3_Sda_READ HAL_GPIO_ReadPin(Sht30_I2c3_Sda_GPIO,Sht30_I2c3_Sda_PIN)


// SDA on port B, bit 14
#define SDA_LOW()	Sht30_I2c3_Sda_CLR // set SDA to low
#define SDA_OPEN()  Sht30_I2c3_Sda_SET // set SDA to open-drain
#define SDA_READ	Sht30_I2c3_Sda_READ	// read SDA

// SCL on port B, bit 13	/* -- adapt the defines for your uC -- */ 
#define SCL_LOW()	Sht30_I2c3_Scl_CLR // set SCL to low
#define SCL_OPEN()  Sht30_I2c3_Scl_SET // set SCL to open-drain
#define SCL_READ	Sht30_I2c3_Scl_READ	// read SCL

//-- Static function prototypes ----------------------------------------------- 
static etError I2c3_WaitWhileClockStreching(u8t timeout);

//-----------------------------------------------------------------------------
void I2c3_Init(void)	/* -- adapt the init for your uC -- */
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */

  __HAL_RCC_GPIOB_CLK_ENABLE();
    

	
 	//RCC_APB2PeriphClockCmd(Sht30_I2c3_Sda_ClockCmd | Sht30_I2c3_Scl_ClockCmd, ENABLE);
	
	GPIO_InitStruct.Pin = Sht30_I2c3_Sda_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;//开漏输出
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(Sht30_I2c3_Sda_GPIO, &GPIO_InitStruct);
    

	HAL_GPIO_WritePin(Sht30_I2c3_Sda_GPIO, Sht30_I2c3_Sda_PIN, GPIO_PIN_SET);
    
	GPIO_InitStruct.Pin = Sht30_I2c3_Scl_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;//开漏输出
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(Sht30_I2c3_Scl_GPIO, &GPIO_InitStruct);
    
	HAL_GPIO_WritePin(Sht30_I2c3_Scl_GPIO, Sht30_I2c3_Scl_PIN, GPIO_PIN_SET);
}

//-----------------------------------------------------------------------------
void I2c3_StartCondition(void)
{
    SDA_OPEN();
    Delay_us(1);
    SCL_OPEN();
    Delay_us(1);
    SDA_LOW();
    Delay_us(10);	// hold time start condition (t_HD;STA)
    SCL_LOW();
    Delay_us(10);
}

//-----------------------------------------------------------------------------
void I2c3_StopCondition(void)
{
    SCL_LOW();
    Delay_us(1);
    SDA_LOW();
    Delay_us(1);
    SCL_OPEN();
    Delay_us(10);	// set-up time stop condition (t_SU;STO)
    SDA_OPEN();
    Delay_us(10);
}

uint8_t I2c3_WaitAck(void)
{
	uint8_t ucErrTime = 0;

		SDA_OPEN();	/* CPU释放SDA总线 */
		Delay_us(10);
		SCL_OPEN();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
		Delay_us(10);
		while (SDA_READ)
		{
			ucErrTime++;
			if (ucErrTime > 250)
			{
				I2c3_StopCondition();
				return 1;
			}
		}
		SCL_LOW();
		Delay_us(1);
		return 0;
}

//-----------------------------------------------------------------------------
etError I2c3_WriteByte(u8t txByte)
{
    etError error = NO_ERROR; 
    u8t	mask;
    for(mask = 0x80; mask > 0; mask >>= 1)// shift bit for masking (8 times)
    {
        if((mask & txByte) == 0) SDA_LOW(); // masking txByte, write bit to SDA-Line
        else	SDA_OPEN();
        Delay_us(1);	// data set-up time (t_SU;DAT)
        SCL_OPEN();	          // generate clock pulse on SCL
        Delay_us(5);	// SCL high time (t_HIGH)
        SCL_LOW();
        Delay_us(1);	// data hold time(t_HD;DAT)
    }
    SDA_OPEN();	          // release SDA-line
    SCL_OPEN();	          // clk #9 for ack
    Delay_us(1);	// data set-up time (t_SU;DAT)
    if(SDA_READ) error = ACK_ERROR;	// check ack from I2c3 slave
    SCL_LOW();
    Delay_us(20);	// wait to see byte package on scope
    return error;	          // return error code
}

//-----------------------------------------------------------------------------
etError I2c3_ReadByte(u8t *rxByte, etI2c3Ack ack, u8t timeout)
{
    etError error = NO_ERROR; 
    u8t mask;
    *rxByte = 0x00;
    SDA_OPEN();	// release SDA-line
    for(mask = 0x80; mask > 0; mask >>= 1) // shift bit for masking (8 times)
    {
        SCL_OPEN();	// start clock on SCL-line
        Delay_us(1);	// clock set-up time (t_SU;CLK)
        error = I2c3_WaitWhileClockStreching(timeout);// wait while clock streching
        Delay_us(3);   //	SCL high time (t_HIGH)
        if(SDA_READ) *rxByte |= mask;	//read bit
        SCL_LOW();	        
        Delay_us(1);		//	data hold time(t_HD;DAT)
    }			
    if(ack == ACK3) SDA_LOW();		//	send acknowledge if necessary
    else	SDA_OPEN();			
    Delay_us(1);		//	data set-up time (t_SU;DAT)
    SCL_OPEN();		 //	clk #9 for ack
    Delay_us(5);		//	SCL high time (t_HIGH)
    SCL_LOW();			
    SDA_OPEN();		 //	release SDA-line
    Delay_us(20);//	wait to see byte package on scope

    return error;		//return with no error
}

//-----------------------------------------------------------------------------
etError I2c3_GeneralCallReset(void)
{
    etError error;

    I2c3_StartCondition();
    error = I2c3_WriteByte(0x00);
    if(error == NO_ERROR) error = I2c3_WriteByte(0x06);

    return error;
}

//-----------------------------------------------------------------------------
static etError I2c3_WaitWhileClockStreching(u8t timeout)
{
    etError error = NO_ERROR;

    while(SCL_READ == 0)
    {
        if(timeout-- == 0) return TIMEOUT_ERROR; 
        Delay_us(1000);
    }
    
    return error;
}
