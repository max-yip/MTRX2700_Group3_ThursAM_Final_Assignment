/*
 * ptu_servo.h
 *
 *  Created on: May 3, 2025
 *      Author: maxyi
 */

#ifndef INC_PTU_SERVO_H_
#define INC_PTU_SERVO_H_

#include "stm32f3xx_hal.h"
#include "serial.h"
#include "ptu_definitions.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

HAL_StatusTypeDef initialise_ptu_pwm(TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2);

void setServoPWM(uint16_t vertical_PWM, uint16_t horizontal_PWM);

void servo_command_parser(SerialPort *serial_port);

#endif /* INC_PTU_SERVO_H_ */
