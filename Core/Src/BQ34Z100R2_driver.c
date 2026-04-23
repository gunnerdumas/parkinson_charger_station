#include "BQ34Z100R2_driver.h"

uint8_t init_BQ34Z(BQ34Z100Z *dev, I2C_HandleTypeDef *i2cHandle)
{
	dev->i2cHandle = i2cHandle;
	dev->soc = 0;
	dev->voltage = 0;
	return 0;
}

//DATA
HAL_StatusTypeDef BQ34Z100R4_Read_SOC(BQ34Z100Z *dev)
{
	uint8_t socData = 0;
	HAL_StatusTypeDef status = BQ34Z100Z_Read_Register(dev, BQ34_SOC, &socData);
	dev->soc = socData;
	return status;
}

HAL_StatusTypeDef BQ34Z100Z_Read_Voltage(BQ34Z100Z *dev)
{
	uint8_t voltage[2];
	HAL_StatusTypeDef status = BQ34Z100Z_Read_Register(dev, BQ34_VOLT, voltage);
	dev->soc = voltage[0] << 8 | voltage[1];
	return status;
}

//Register control
HAL_StatusTypeDef BQ34Z100Z_Read_Register(BQ34Z100Z *dev, uint8_t reg, uint8_t *data)
{
	HAL_I2C_Mem_Read(dev->i2cHandle, BQ34Z100R2_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef BQ34Z100Z_Read_Registers(BQ34Z100Z *dev, uint8_t reg, uint8_t *data, uint8_t length)
{
	HAL_I2C_Mem_Read(dev->i2cHandle, BQ34Z100R2_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

HAL_StatusTypeDef BQ34Z100Z_Write_Register(BQ34Z100Z *dev, uint8_t reg, uint8_t *data)
{
	HAL_I2C_Mem_Write(dev->i2cHandle, BQ34Z100R2_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}
