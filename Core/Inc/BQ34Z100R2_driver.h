#ifndef BQ34Z100R2_DRIVER_H
#define BQ34Z100R2_DRIVER_H

#include "stm32l4xx_hal.h"
/*
Driver for the BQ34Z100-R2 TI Battery monitor used in an stm32 enviroment part
of the parkinson charger

Author: Gunner Cook-Dumas
Date created: 4/20/26
*/
//target ADDRESS 1010101+r/w(1/0)
#define BQ34Z100R2_ADDR (0b1010101<<1)

//each command consists of two bytes of data, two consecutive HDQ/I2C transmissions must be executed to
//initiate the command function and to read or write the corresponding two bytes of data.

//STANDERD DATA COMMANDS
#define BQ34_CNTL                   0x00 //0x00-0x01
#define BQ34_SOC                    0x02 //0x02
#define BQ34_ME                     0x03 //0x03
#define BQ34_RM                     0x04 //0x04-0x05
#define BQ34_FCC                    0x06 //0x06-0x07
#define BQ34_VOLT                   0x08 //0x08-0x09
#define BQ34_AI                     0x0A //0x0A-0x0B
#define BQ34_TEMP                   0x0C //0x0C-0x0D
#define BQ34_FLAGS                  0x0E //0x0E-0x0F
#define BQ34_I                      0x10 //0x10-0x11
#define BQ34_FLAGSB                 0x12 //0x12-0x13

//FLASH CONTROLS
#define BQ34_BLOCK_DATA_CTNL		0x61
#define BQ43_DATA_FLASH_CLASS		0x3E
#define BQ34_DATA_FLASH_BLK			0x3F

//SUB CTNL COMMANDS
#define BQ34_CNTL_STAT              0x0000
#define BQ34_CNTL_DEVICE_TYPE       0x0001
#define BQ34_CNTL_FW_VERSION        0x0002
#define BQ34_CNTL_HW_VERSION        0x0003
#define BQ34_CNTL_RESET_DATA        0x0005
#define BQ34_CNTL_PREVWRITE         0x0007
#define BQ34_CNTL_CHEM_ID           0x0008
#define BQ34_CNTL_BRD_OFFSET        0x0009
#define BQ34_CNTL_CC_OFFSET         0x000A
#define BQ34_CNTL_CC_OFFSET_SAVE    0x000B
#define BQ34_CNTL_DF_VERSION        0x000C
#define BQ34_CNTL_SET_FULLSLEEP     0x0010
#define BQ34_CNTL_STATIC_CHEM_CHKSUM 0x0017
#define BQ34_CNTL_SEALED            0x0020
#define BQ34_CNTL_IT_ENABLE         0x0021
#define BQ34_CNTL_CAL_ENABLE        0x002D
#define BQ34_CNTL_RESET             0x0041
#define BQ34_CNTL_EXIT_CAL          0x0080
#define BQ34_CNTL_ENTER_CAL         0x0081
#define BQ34_CNTL_OFFSET_CAL        0x0082

typedef struct{
	I2C_HandleTypeDef *i2cHandle;
	uint8_t soc;
	uint16_t voltage;
	uint16_t capacity;
	uint8_t flagsA[2];
	uint8_t flagsB[2];
}BQ34Z100Z;

//Initlzation
uint8_t init_BQ34Z(BQ34Z100Z *dev, I2C_HandleTypeDef *i2cHandle);
HAL_StatusTypeDef BQ34Z100Z_Unseal(BQ34Z100Z *dev);
HAL_StatusTypeDef BQ34Z100Z_Seal(BQ34Z100Z *dev);

//Write flash
//CAUTION WITH THIS DONT WRITE TO MANY TIMES
HAL_StatusTypeDef BQ34Z100Z_BatteryConfigueration(BQ34Z100Z *dev);

//DATA
HAL_StatusTypeDef BQ34Z100R4_Read_SOC(BQ34Z100Z *dev);
HAL_StatusTypeDef BQ34Z100Z_Read_Voltage(BQ34Z100Z *dev);
HAL_StatusTypeDef BQ34Z100Z_Read_Capacity(BQ34Z100Z *dev);
HAL_StatusTypeDef BQ34Z100Z_Read_Flags(BQ34Z100Z *dev);
HAL_StatusTypeDef BQ34Z100Z_Read_Full_Chg(BQ34Z100Z *dev);

//Register control
HAL_StatusTypeDef BQ34Z100Z_Read_Register(BQ34Z100Z *dev, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef BQ34Z100Z_Read_Registers(BQ34Z100Z *dev, uint8_t reg, uint8_t *data, uint8_t length);
HAL_StatusTypeDef BQ34Z100Z_Write_Register(BQ34Z100Z *dev, uint8_t reg, uint8_t *data, uint8_t length);



#endif
