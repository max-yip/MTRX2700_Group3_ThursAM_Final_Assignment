#include "ptu_i2c.h"


ACCELEROMETER_CFG_STRUCT accelerometer_cfg = {ADXL345_POWER_CTL, 0x08, ADXL345_DATA_FORMAT, 0x08};

//// get gyro data
//int16_t yaw_rate = 0, pitch_rate = 0, roll_rate = 0;
//read_gyro_data(&hi2c1, &yaw_rate, &pitch_rate, &roll_rate);
//
//// get accel data
//int16_t acc_x = 0, acc_y = 0, acc_z = 0;
//read_accel_data(&hi2c1, &acc_x, &acc_y, &acc_z);


void initialise_ptu_i2c(I2C_HandleTypeDef *i2c) {

	HAL_StatusTypeDef return_value = 0x00;


	// === Gyroscope initialise ===
	uint8_t reg_1 = 0b00001111;
	// Enable x, y, z and turn off power down:
	return_value = HAL_I2C_Mem_Write(i2c, gyro_wr, L3G4200D_CTRL_REG1, 1, &reg_1, 1, 10);

	uint8_t reg_2 = 0b00000000;
	// If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
	return_value = HAL_I2C_Mem_Write(i2c, gyro_wr, L3G4200D_CTRL_REG2, 1, &reg_2, 1, 10);

	// Configure CTRL_REG3 to generate data ready interrupt on INT2
	// No interrupts used on INT1, if you'd like to configure INT1
	// or INT2 otherwise, consult the datasheet:
	return_value = HAL_I2C_Mem_Write(i2c, gyro_wr, L3G4200D_CTRL_REG3, 1, &reg_2, 1, 10);


	// === Accelerometer initialise ===
	// Power on and set to measurement mode
	return_value = HAL_I2C_Mem_Write(i2c, accel_wr, accelerometer_cfg.power_ctl_register, 1, &accelerometer_cfg.power_ctl_value, 1, 100);

	// Set data format (e.g., full resolution, +/-2g)
	return_value = HAL_I2C_Mem_Write(i2c, accel_wr, accelerometer_cfg.data_format_register, 1, &accelerometer_cfg.data_format_value, 1, 100);


	// CTRL_REG4 controls the full-scale range, among other things:
	//if(scale == 250){
	////////HAL_I2C_Mem_Write(&hi2c1, gyro_wr, L3G4200D_CTRL_REG4, 1, &reg_2, 1, 10);
	//}else if(scale == 500){
	//writeRegister(gyro_Address, gyro_CTRL_REG4, 0b00010000);
	//}else{
	//writeRegister(gyro_Address, gyro_CTRL_REG4, 0b00110000);
	//}

	// CTRL_REG5 controls high-pass filtering of outputs, use it
	// if you'd like:
	//HAL_I2C_Mem_Write(&hi2c1, gyro_wr, L3G4200D_CTRL_REG5, 1, &reg_2, 10);

	// === lidar initialise === // doesnt work
//	uint8_t reset_value = 0x00;
//	return_value = HAL_I2C_Mem_Write(i2c, LIDAR_WR, 0x00, 1, &reset_value, 1, 10);
}


void read_gyro_data (I2C_HandleTypeDef *i2c, int16_t *yaw, int16_t *pitch, int16_t *roll){
	uint8_t xMSB = 0x00, xLSB = 0x00;
	HAL_I2C_Mem_Read(i2c, gyro_rd, 0x29, 1, &xMSB, 1, 10);
	HAL_I2C_Mem_Read(i2c, gyro_rd, 0x28, 1, &xLSB, 1, 10);
	*yaw = ((int16_t)(xMSB << 8) | xLSB);

	uint8_t yMSB = 0x00, yLSB = 0x00;
	HAL_I2C_Mem_Read(i2c, gyro_rd, 0x2B, 1, &yMSB, 1, 10);
	HAL_I2C_Mem_Read(i2c, gyro_rd, 0x2A, 1, &yLSB, 1, 10);
	*pitch = ((int16_t)(yMSB << 8) | yLSB);

	uint8_t zMSB = 0x00, zLSB = 0x00;
	HAL_I2C_Mem_Read(i2c, gyro_rd, 0x2D, 1, &zMSB, 1, 10);
	HAL_I2C_Mem_Read(i2c, gyro_rd, 0x2C, 1, &zLSB, 1, 10);
	*roll = ((int16_t)(zMSB << 8) | zLSB);
}



void read_accel_data(I2C_HandleTypeDef *i2c, int16_t *acc_x, int16_t *acc_y, int16_t *acc_z){
	uint8_t accel_data[6] = {0};
	HAL_I2C_Mem_Read(i2c, accel_rd, ADXL345_DATAX0, 1, accel_data, 6, 100);
	// Combine LSB and MSB
	*acc_x = (int16_t)((accel_data[1] << 8) | accel_data[0]);
	*acc_y = (int16_t)((accel_data[3] << 8) | accel_data[2]);
	*acc_z = (int16_t)((accel_data[5] << 8) | accel_data[4]);
}

