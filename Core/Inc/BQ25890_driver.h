#ifndef BQ25890_DRIVER_H
#define BQ25890_DRIVER_H
#include "stm32l4xx_hal.h"

/*
Driver for the BQ25890 TI sys lipo charger used in an stm32 environment part
of the parkinson charger

Author: Gunner Cook-Dumas
Date created: 4/20/26
*/

//target address 1101010+r/w(1/0)
#define BQ25890_ADDR (0b1101010 << 1)

//Registers
//The charger device supports multi-read and multi-write on REG00 through REG14 except REG0C.
//REG0C is a fault register.

#define BQ25890_REG00 0x00
#define BQ25890_REG01 0x01
#define BQ25890_REG02 0x02
#define BQ25890_REG03 0x03
#define BQ25890_REG04 0x04
#define BQ25890_REG05 0x05
#define BQ25890_REG06 0x06
#define BQ25890_REG07 0x07
#define BQ25890_REG08 0x08
#define BQ25890_REG09 0x09
#define BQ25890_REG0A 0x0A
#define BQ25890_REG0B 0x0B
#define BQ25890_REG0C 0x0C
#define BQ25890_REG0D 0x0D
#define BQ25890_REG0E 0x0E
#define BQ25890_REG0F 0x0F
#define BQ25890_REG10 0x10
#define BQ25890_REG11 0x11
#define BQ25890_REG12 0x12
#define BQ25890_REG13 0x13
#define BQ25890_REG14 0x14

typedef struct
{
	I2C_HandleTypeDef *i2cHandle;
	uint16_t battVoltage; //mV
	uint16_t sysVoltage; //mV
	uint16_t vbusVoltage; //mV
	uint16_t chargeI; //mA
	uint8_t temp;
	uint8_t flagB;
	uint8_t flagFaults;
	uint8_t usbStatus;
}BQ25890;


//setup
uint8_t init_BQ25890(BQ25890 *dev, I2C_HandleTypeDef *i2cHandle);
HAL_StatusTypeDef BQ25890_WatchDog_Disable(BQ25890 *dev);
HAL_StatusTypeDef BQ25890_Set_Voltage(BQ25890 *dev);
HAL_StatusTypeDef BQ25890_Set_Current(BQ25890 *dev);
HAL_StatusTypeDef BQ25890_Enable_Charge(BQ25890 *dev);

//data
HAL_StatusTypeDef BQ25890_Flags(BQ25890 *dev);
HAL_StatusTypeDef BQ25890_Read_Faults(BQ25890 *dev);
HAL_StatusTypeDef BQ25890_Get_Batt_Voltage(BQ25890 *dev);
HAL_StatusTypeDef BQ25890_Get_Temp(BQ25890 *dev);
HAL_StatusTypeDef BQ25890_Get_Sys_Voltage(BQ25890 *dev);
HAL_StatusTypeDef BQ25890_Get_VBUS(BQ25890 *dev);
HAL_StatusTypeDef BQ25890_Get_Charge_I(BQ25890 *dev);
HAL_StatusTypeDef BQ25890_Get_Charge_Status(BQ25890 *dev);
HAL_StatusTypeDef BQ25890_Get_USB_Status(BQ25890 *dev);


//register control
HAL_StatusTypeDef BQ25890_Read_Register(BQ25890 *dev, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef BQ25890_Read_Registers(BQ25890 *dev, uint8_t reg, uint8_t *data, uint8_t length);
HAL_StatusTypeDef BQ25890_Write_Register(BQ25890 *dev, uint8_t reg, uint8_t *data, uint8_t length);

#endif BQ25890_DRIVER_H
