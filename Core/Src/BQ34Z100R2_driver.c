#include "BQ34Z100R2_driver.h"

uint8_t init_BQ34Z(BQ34Z100Z *dev, I2C_HandleTypeDef *i2cHandle)
{
	dev->i2cHandle = i2cHandle;
	dev->soc = 0;
	dev->voltage = 0;
	dev->capacity = 0;
	dev->flagsA[0] = 0;
	dev->flagsA[1] = 0;
	dev->flagsB[0] = 0;
	dev->flagsB[1] = 0;
	BQ34Z100Z_Read_Flags(dev);
//	BQ34Z100Z_Unseal(dev);
//
//	//run sparingly as controls the flash and
//	//limited amount of writes
//	BQ34Z100Z_BatteryConfigueration(dev);
//
//	BQ34Z100Z_Seal(dev);
	return 0;
}

HAL_StatusTypeDef BQ34Z100Z_Unseal(BQ34Z100Z *dev)
{
	HAL_StatusTypeDef status;

	//Read the control status reg first
	uint8_t cntl_status[2] = {0};
	uint8_t subCmd[] = {0x00, 0x00};


	status = BQ34Z100Z_Write_Register(dev, BQ34_CNTL, subCmd, 2);
	if(status != HAL_OK)
		return status;

	status = BQ34Z100Z_Read_Registers(dev, BQ34_CNTL, cntl_status, 2);
	if(status != HAL_OK)
		return status;

	//check if bits 6 and 5 set FAS, SS so that in sealed state
	if((0x6 & cntl_status[1])!=0)
	{
		status = HAL_ERROR;
		return status;
	}

	//Send the low-byte Unseal Key (the low byte is 0414; the default Unseal Key is 0x36720414)
	//Send the high-byte Unseal Key (the high byte is 3672; the default Unseal Key is 0x36720414).
	uint8_t lowKey[] = {0x14, 0x04};
	uint8_t highKey[] = {0x72, 0x36};

	status = BQ34Z100Z_Write_Register(dev, BQ34_CNTL, lowKey, 2);
	if (status != HAL_OK)
		return status;

	status = BQ34Z100Z_Write_Register(dev, BQ34_CNTL, highKey, 2);
	if (status != HAL_OK)
		return status;

	//wait a moment to set
	HAL_Delay(100);

	status = BQ34Z100Z_Write_Register(dev, BQ34_CNTL, subCmd, 2);
	if(status != HAL_OK)
		return status;

	status = BQ34Z100Z_Read_Registers(dev, BQ34_CNTL, cntl_status, 2);
	if(status != HAL_OK)
		return status;


	return HAL_OK;
}

HAL_StatusTypeDef BQ34Z100Z_Seal(BQ34Z100Z *dev)
{
	HAL_StatusTypeDef status;

	uint8_t subCmd[]={0x20, 0x00};
	uint8_t cntl_status[2] = {0};


	status = BQ34Z100Z_Write_Register(dev, BQ34_CNTL, subCmd, 2);
	if(status != HAL_OK)
		return status;

	HAL_Delay(200);
	status = BQ34Z100Z_Write_Register(dev, BQ34_CNTL, subCmd, 2);
	if(status != HAL_OK)
		return status;

	status = BQ34Z100Z_Read_Registers(dev, BQ34_CNTL, cntl_status, 2);
	if(status != HAL_OK)
		return status;

	return HAL_OK;
}

HAL_StatusTypeDef BQ34Z100Z_BatteryConfigueration(BQ34Z100Z *dev)
{
	HAL_StatusTypeDef status;
	uint8_t blockData[32];
	//Enable block access
	uint8_t mode = 0x00;
	status = BQ34Z100Z_Write_Register(dev, BQ34_BLOCK_DATA_CTNL, &mode, 1);
	if(status != HAL_OK)
		return status;

	//select configuration class id=48(0x30)
	uint8_t classID = 0x30;
	status = BQ34Z100Z_Write_Register(dev, BQ43_DATA_FLASH_CLASS, &classID, 1);
	if(status != HAL_OK)
		return status;

	//select block 0 and read the next 32 blocks
	uint8_t blockID = 0;
	status = BQ34Z100Z_Write_Register(dev, BQ34_DATA_FLASH_BLK, &blockID, 1);
	if(status != HAL_OK)
		return status;

	status = BQ34Z100Z_Read_Registers(dev, 0x40, blockData, 32);
	if(status != HAL_OK)
		return status;

	//modify the buffer to my values
	//0 I2 Design Voltage 4200mV 0x1068
	blockData[0] = 0x10; blockData[1] = 0x68;
	//11 I2 Design Capacity 12000mV 0x2EE0
	blockData[11] = 0x2E; blockData[12] = 0xE0;
	//13 I2 Design Energy 5040cWh 0x13B0
	blockData[13] = 0x13; blockData[14] = 0xB0;

	//write the block back into flash
	status = BQ34Z100Z_Write_Register(dev, 0x40, blockData, 32);
	if(status != HAL_OK)
		return status;

	//checksum 0x60
	uint8_t sum = 0;
	for(uint8_t i = 0; i < 32; i++)
	{
		sum += blockData[i];
	}
	uint8_t checksum = 255-sum;
	status = BQ34Z100Z_Write_Register(dev, 0x60, &checksum, 1);
	if(status != HAL_OK)
		return status;

//	Reset the gauge to ensure the new data flash parameter goes into effect by using Control(0x0041).
	uint8_t reset[] = {0x41, 0x00};
	status = BQ34Z100Z_Write_Register(dev, BQ34_CNTL, reset, 2);
	if(status != HAL_OK)
		return status;
	return HAL_OK;
}


//DATA
HAL_StatusTypeDef BQ34Z100R4_Read_SOC(BQ34Z100Z *dev)
{
	uint8_t socData = 0;
	HAL_StatusTypeDef status = BQ34Z100Z_Read_Register(dev, BQ34_SOC, &socData);
	if(status != HAL_OK)
		return status;
	dev->soc = socData;
	return status;
}

//this returns only 0xffff, indiating an error but cannot diagonse. so dont use
HAL_StatusTypeDef BQ34Z100Z_Read_Voltage(BQ34Z100Z *dev)
{
	uint8_t voltage[2]={0};
//	HAL_StatusTypeDef status = BQ34Z100Z_Read_Register(dev, BQ34_VOLT, voltage);
	HAL_StatusTypeDef status = BQ34Z100Z_Read_Registers(dev, BQ34_VOLT, voltage, 2);
	if(status != HAL_OK)
		return status;
	dev->voltage = voltage[1] << 8 | voltage[0];
	return status;
}

HAL_StatusTypeDef BQ34Z100Z_Read_Capacity(BQ34Z100Z *dev)
{
	HAL_StatusTypeDef status;
	uint8_t capacity[2] = {0};
	status = BQ34Z100Z_Read_Registers(dev, BQ34_RM, capacity, 2);
	if(status != HAL_OK)
		return status;
	dev->capacity = capacity[1] << 8 | capacity[0];
	return status;
}

HAL_StatusTypeDef BQ34Z100Z_Read_Flags(BQ34Z100Z *dev)
{
	HAL_StatusTypeDef status;
	uint8_t flagA[2] = {0};
	uint8_t flagB[2] = {0};
	status = BQ34Z100Z_Read_Registers(dev, BQ34_FLAGS, flagA, 2);
	if(status != HAL_OK)
		return status;
	status = BQ34Z100Z_Read_Registers(dev, BQ34_FLAGSB, flagB, 2);
	if(status != HAL_OK)
		return status;

	dev->flagsA[0] = flagA[0];
	dev->flagsA[1] = flagA[1];
	dev->flagsB[0] = flagB[0];
	dev->flagsB[1] = flagB[1];

	return status;
}

HAL_StatusTypeDef BQ34Z100Z_Read_Full_Chg(BQ34Z100Z *dev)
{
	HAL_StatusTypeDef status;
	uint8_t fullChg[2] = {0};

	status = BQ34Z100Z_Read_Registers(dev, BQ34_FCC, fullChg, 2);
	if(status != HAL_OK)
		return status;
//	uint16_t fullCharge = (uint16_t)(fullChg[1] << 8 | fullChg[0]);
	return status;
}

//Register control
HAL_StatusTypeDef BQ34Z100Z_Read_Register(BQ34Z100Z *dev, uint8_t reg, uint8_t *data)
{
	return HAL_I2C_Mem_Read(dev->i2cHandle, BQ34Z100R2_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef BQ34Z100Z_Read_Registers(BQ34Z100Z *dev, uint8_t reg, uint8_t *data, uint8_t length)
{
	return HAL_I2C_Mem_Read(dev->i2cHandle, BQ34Z100R2_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

HAL_StatusTypeDef BQ34Z100Z_Write_Register(BQ34Z100Z *dev, uint8_t reg, uint8_t *data, uint8_t length)
{
	return HAL_I2C_Mem_Write(dev->i2cHandle, BQ34Z100R2_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}
