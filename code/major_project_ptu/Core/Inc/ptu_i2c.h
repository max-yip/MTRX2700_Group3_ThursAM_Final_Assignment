/*
 * ptu_i2c.h
 *
 *  Created on: Apr 14, 2023
 *      Author: stew
 */

#ifndef INC_PTU_I2C_H_
#define INC_PTU_I2C_H_

#include "stm32f3xx_hal.h"
#include "serial.h"
#include "ptu_definitions.h"
#include <string.h>
#include <ctype.h>


void initialise_ptu_i2c(I2C_HandleTypeDef *i2c);

void read_gyro_data (I2C_HandleTypeDef *i2c, int16_t *yaw, int16_t *pitch, int16_t *roll);

void read_accel_data(I2C_HandleTypeDef *i2c, int16_t *acc_x, int16_t *acc_y, int16_t *acc_z);

#endif /* INC_PTU_I2C_H_ */
