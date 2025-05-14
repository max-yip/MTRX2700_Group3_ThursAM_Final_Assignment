/*
 * ptu_lidar.c
 *
 *  Created on: May 3, 2025
 *      Author: maxyi
 */


#include "ptu_lidar.h"

uint16_t last_capture = 0;
uint16_t diff = 0;

uint16_t rise_time = 0;
uint16_t last_period = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
//	uint8_t buffer[32];
	if (htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		uint16_t IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 1)
			rise_time = IC_Val1;
		else
			last_period = IC_Val1 - rise_time;

		diff = IC_Val1 - last_capture;
		last_capture = IC_Val1;
	}
}


