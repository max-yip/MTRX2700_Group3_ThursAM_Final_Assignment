/*
 * ptu_servo.c
 *
 *  Created on: May 3, 2025
 *      Author: maxyi
 */


#include "ptu_servo.h"


// ==== Helper functions ===
uint8_t isNumeric(const char *str) {
    if (str == NULL || *str == '\0') return 0; // empty or NULL

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) {
            return 0;
        }
    }
    return 1;
}


// ==== initialise servo pwm ===
HAL_StatusTypeDef initialise_ptu_pwm(TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2) {
    HAL_StatusTypeDef return_value;

    // Start PWM on TIM2 Channel 1 and Channel 2
    return_value = HAL_TIM_PWM_Start(htim2, TIM_CHANNEL_1);
    if (return_value != HAL_OK) {
        return return_value;  // Early return if PWM fails to start
    }

    return_value = HAL_TIM_PWM_Start(htim2, TIM_CHANNEL_2);
    if (return_value != HAL_OK) {
        return return_value;  // Early return if PWM fails to start
    }

    // Start Input Capture with interrupt on TIM1 Channel 1
    return_value = HAL_TIM_IC_Start_IT(htim1, TIM_CHANNEL_1);
    if (return_value != HAL_OK) {
        return return_value;  // Early return if input capture fails to start
    }

    // Set TIM2 ARR for 20ms period (50Hz) for servos (1 count = 1us)
    htim2->Instance->ARR = 20000;

    // Enable Auto-Reload Preload to update ARR safely at the end of the PWM cycle
    htim2->Instance->CR1 |= TIM_CR1_ARPE;


    // Delay for LIDAR initialization
    HAL_Delay(100);

    return HAL_OK;  // Success
}



void setServoPWM(uint16_t vertical_PWM, uint16_t horizontal_PWM){
	TIM2->CCR1 = vertical_PWM;
	TIM2->CCR2 = horizontal_PWM;
}



void servo_command_parser(SerialPort *serial_port) {
    char command_line[BUFFER_SIZE];
    strcpy(command_line, serialGetReceivedBuffer(serial_port));


    char *pan = strtok(command_line, ",");
    char *tilt = strtok(NULL, ",");

    if (pan == NULL || tilt == NULL) return;

    if (isNumeric(pan) && isNumeric(tilt)) {
        int pwm1 = atoi(pan);
        int pwm2 = atoi(tilt);

        if (pwm1 >= 1200 && pwm1 <= 1900 && pwm2 >= 1200 && pwm2 <= 1900) {
            TIM2->CCR1 = pwm1;
            TIM2->CCR2 = pwm2;
        }
    }
}


