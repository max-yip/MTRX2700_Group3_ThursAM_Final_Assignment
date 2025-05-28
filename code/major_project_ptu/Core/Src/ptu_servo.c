/*
 * ptu_servo.c
 *
 *  Created on: May 3, 2025
 *      Author: maxyi
 */


#include "ptu_servo.h"

volatile uint16_t vertical_start_PWM = 1551;
volatile uint16_t horizontal_start_PWM = 1551;

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


void setServoPWM(uint16_t vertical_PWM, uint16_t horizontal_PWM){
	TIM2->CCR1 = vertical_PWM;
	TIM2->CCR2 = horizontal_PWM;
}


// ===== Initialize servo PWM and input capture for PTU =====
HAL_StatusTypeDef initialise_ptu_pwm(TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2) {
    HAL_StatusTypeDef return_value;

    // Start PWM on TIM2 Channel 1 (pan servo)
    return_value = HAL_TIM_PWM_Start(htim2, TIM_CHANNEL_1);
    if (return_value != HAL_OK) {
        return return_value;  // Return early on failure
    }

    // Start PWM on TIM2 Channel 2 (tilt servo)
    return_value = HAL_TIM_PWM_Start(htim2, TIM_CHANNEL_2);
    if (return_value != HAL_OK) {
        return return_value;  // Return early on failure
    }

    // Start Input Capture with interrupt on TIM1 Channel 1 (for feedback or sync)
    return_value = HAL_TIM_IC_Start_IT(htim1, TIM_CHANNEL_1);
    if (return_value != HAL_OK) {
        return return_value;  // Return early on failure
    }

    // Configure TIM2 auto-reload for 20ms period (50Hz PWM frequency, 1 count = 1us)
    htim2->Instance->ARR = 20000;

    // Enable Auto-Reload Preload (ARPE) for safe ARR updates at the end of PWM cycle
    htim2->Instance->CR1 |= TIM_CR1_ARPE;

    // Wait for 100ms to allow LIDAR (or other sensors) to initialize
    HAL_Delay(100);

    // Set initial servo PWM pulse widths for vertical and horizontal servos
    setServoPWM(vertical_start_PWM, horizontal_start_PWM);

    return HAL_OK;  // Initialization successful
}



// ===== Parse and apply servo commands from serial input =====
void servo_command_parser(SerialPort *serial_port) {
    char command_line[BUFFER_SIZE];

    // Copy received buffer into local command line string
    strcpy(command_line, serialGetReceivedBuffer(serial_port));

    // Parse pan and tilt PWM values separated by comma
    char *pan = strtok(command_line, ",");
    char *tilt = strtok(NULL, ",");

    // Validate that both values exist
    if (pan == NULL || tilt == NULL) return;

    // Check if both parsed strings are numeric
    if (isNumeric(pan) && isNumeric(tilt)) {
        int pwm1 = atoi(pan);
        int pwm2 = atoi(tilt);

        // Validate PWM value ranges before applying to servo registers
        if (pwm1 >= 1500 && pwm1 <= 2300 && pwm2 >= 1200 && pwm2 <= 1900) {
            TIM2->CCR1 = pwm1;  // Set pan servo PWM duty cycle
            TIM2->CCR2 = pwm2;  // Set tilt servo PWM duty cycle
        }
    }
}
