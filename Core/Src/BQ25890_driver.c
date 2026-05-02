#include "BQ25890_driver.h"

//Setup
uint8_t init_BQ25890(BQ25890 *dev, I2C_HandleTypeDef *i2cHandle)
{
	dev->i2cHandle = i2cHandle;
	dev->battVoltage = 0;
	dev->flagB = 0;
	dev->flagFaults = 0;
	dev->temp = 0;
	dev->chargeI = 0;
	dev->sysVoltage = 0;
	dev->vbusVoltage = 0;

	HAL_StatusTypeDef status;
	status = BQ25890_WatchDog_Disable(dev);
	if(status != HAL_OK)
		return 25;

	status = BQ25890_Set_Current(dev);
	if(status != HAL_OK)
		return 50;

	status = BQ25890_Set_Voltage(dev);
	if(status != HAL_OK)
		return 75;

	//setup some extra varibles
	//minimu sys voltage limit set to 3.2Volts and enable auto charge
	uint8_t cmd = 0;
	status = BQ25890_Read_Register(dev, BQ25890_REG03, &cmd);
	cmd = cmd | 0x14;
	status = BQ25890_Write_Register(dev, BQ25890_REG03, &cmd, 1);
	if(status != HAL_OK)
		return 100;

	//enalbe contiouns batter conversion require 1 second bootup
	cmd = 0;
	status = BQ25890_Read_Register(dev, BQ25890_REG02, &cmd);
	cmd = cmd | (1 << 6);
	status = BQ25890_Write_Register(dev, BQ25890_REG02, &cmd, 1);
	if(status != HAL_OK)
		return 125;
	HAL_Delay(1000);

	status = BQ25890_Flags(dev);
	if(status != HAL_OK)
		return 150;

	return 0;
}

HAL_StatusTypeDef BQ25890_WatchDog_Disable(BQ25890 *dev)
{
	HAL_StatusTypeDef status;
	uint8_t cmd = 0x00;
	status = BQ25890_Read_Register(dev, BQ25890_REG07, &cmd);
	if(status != HAL_OK)
		return status;

	//clear the bits 5 and 4 Watchdong and set charing termination disapbled
	cmd = cmd & 0xCF;
	cmd = cmd | 0x00;
	status = BQ25890_Write_Register(dev, BQ25890_REG07, &cmd, 1);
	if(status != HAL_OK)
		return status;

	//check for sanity
//	status = BQ25890_Read_Register(dev, BQ25890_REG07, &cmd);
	return HAL_OK;
}

HAL_StatusTypeDef BQ25890_Set_Voltage(BQ25890 *dev)
{
	HAL_StatusTypeDef status;
	uint8_t cmd = 0x00;
	status = BQ25890_Read_Register(dev, BQ25890_REG06, &cmd);
	if(status != HAL_OK)
		return status;

	//set voltage to 4.208V
	cmd = cmd & 0x5F;
	status = BQ25890_Write_Register(dev, BQ25890_REG06, &cmd, 1);
	if(status != HAL_OK)
		return status;

	//sanity check
//	status = BQ25890_Read_Register(dev, BQ25890_REG06, &cmd);
	return HAL_OK;
}

HAL_StatusTypeDef BQ25890_Set_Current(BQ25890 *dev)
{
	HAL_StatusTypeDef status;
	uint8_t cmd = 0x00;
	status = BQ25890_Read_Register(dev, BQ25890_REG04, &cmd);
	if(status != HAL_OK)
		return status;

	cmd = cmd & 0x80; //clear all bits precaution
	cmd = cmd | 0xA6; //set charge to 2.432 mA

	status = BQ25890_Write_Register(dev, BQ25890_REG04, &cmd, 1);
	if(status != HAL_OK)
		return status;

	//sanity check
//	status = BQ25890_Read_Register(dev, BQ25890_REG04, &cmd);
	return HAL_OK;

}

//RETURN DATA
HAL_StatusTypeDef BQ25890_Flags(BQ25890 *dev)
{
	HAL_StatusTypeDef status;
	uint8_t buffer = 0x00;

	//get the bits for the vbus and charging
	status = BQ25890_Read_Register(dev, BQ25890_REG0B, &buffer);
	if(status != HAL_OK)
		return status;
	dev->flagB = buffer;

	//get fault flags
	status = BQ25890_Read_Register(dev, BQ25890_REG0C, &buffer);
	if(status != HAL_OK)
		return status;
	dev->flagFaults = buffer;

	return HAL_OK;
}

HAL_StatusTypeDef BQ25890_Get_Batt_Voltage(BQ25890 *dev)
{
	HAL_StatusTypeDef status;
	uint8_t buffer = 0;

	status = BQ25890_Read_Register(dev, BQ25890_REG0E, &buffer);
	if(status != HAL_OK)
		return status;

	//clear top bit
	buffer = buffer & 0x7F;

	//begin conversion all in mV
	dev->battVoltage = (uint16_t)(2304 + (buffer * 20));

	return HAL_OK;
}

HAL_StatusTypeDef BQ25890_Get_Temp(BQ25890 *dev)
{
	HAL_StatusTypeDef status;
	uint8_t buffer = 0;
	status = BQ25890_Read_Register(dev, BQ25890_REG10, &buffer);
	if(status != HAL_OK)
		return status;

	dev->temp = buffer & 0x7F;
	return HAL_OK;
}

HAL_StatusTypeDef BQ25890_Get_Sys_Voltage(BQ25890 *dev)
{
	HAL_StatusTypeDef status;
	uint8_t buffer = 0;
	status = BQ25890_Read_Register(dev, BQ25890_REG0F, &buffer);
	if(status != HAL_OK)
		return status;

	//clear top big
	buffer = buffer & 0x7F;

	//convet to dec all in mV
	dev->sysVoltage = (uint16_t)(2304 + (buffer * 20));

	return HAL_OK;
}

HAL_StatusTypeDef BQ25890_Get_VBUS(BQ25890 *dev)
{
	HAL_StatusTypeDef status;
	uint8_t buffer = 0;
	status = BQ25890_Read_Register(dev, BQ25890_REG11, &buffer);
	if(status != HAL_OK)
		return status;

	if(buffer & 0x80 == 0x80)//vbus not attached
		dev->vbusVoltage = 0;
	else //vbus attach get mesument
		dev->vbusVoltage = buffer & 0x7F;

	return HAL_OK;
}

HAL_StatusTypeDef BQ25890_Get_Charge_I(BQ25890 *dev)
{
	HAL_StatusTypeDef status;
	uint8_t buffer = 0;
	status = BQ25890_Read_Register(dev, BQ25890_REG12, &buffer);
	if(status != HAL_OK)
		return status;

	dev->chargeI = buffer;
	return HAL_OK;

}

HAL_StatusTypeDef BQ25890_Get_Charge_Status(BQ25890 *dev)
{
	HAL_StatusTypeDef status;
	uint8_t buffer = 0;
	status = BQ25890_Read_Register(dev, BQ25890_REG0B, &buffer);
	if(status != HAL_OK)
		return status;

	return HAL_OK;
}

HAL_StatusTypeDef BQ25890_Get_USB_Status(BQ25890 *dev)
{
	HAL_StatusTypeDef status;
	uint8_t buffer = 0;
	status = BQ25890_Read_Register(dev, BQ25890_REG0B, &buffer);
	if(status != HAL_OK)
		return status;

	dev->usbStatus = (buffer & 0xE0) >> 5;

	return HAL_OK;


}

//Register control
HAL_StatusTypeDef BQ25890_Read_Register(BQ25890 *dev, uint8_t reg, uint8_t *data)
{
	return HAL_I2C_Mem_Read(dev->i2cHandle, BQ25890_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef BQ25890_Read_Registers(BQ25890 *dev, uint8_t reg, uint8_t *data, uint8_t length)
{
	return HAL_I2C_Mem_Read(dev->i2cHandle, BQ25890_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

HAL_StatusTypeDef BQ25890_Write_Register(BQ25890 *dev, uint8_t reg, uint8_t *data, uint8_t length)
{
	return HAL_I2C_Mem_Write(dev->i2cHandle, BQ25890_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}
