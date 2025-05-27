/*
 * gpio.h
 *
 *  Created on: May 24, 2025
 *      Author: Paul Akle
 */

#ifndef GPIO_H
#define GPIO_H

#include "stm32f303xc.h"
#include "stm32f3xx_hal.h"

// ----- Servo GPIO Helpers -----

/** Enable clocks for servo PWM: GPIOA and TIM2. */
void enable_servo_clocks(void);

/** Configure PA15 as AF1 for TIM2_CH1 (servo PWM output). */
void configure_servo_gpio(void);

// ----- Beam Sensor GPIO Helpers -----

/** Enable clocks for beam sensor (PC1) and LEDs (PE8..PE15). */
void enable_beam_clocks(void);

/** Configure PC1 as EXTI1 falling-edge input with pull-up. */
void configure_beam_sensor_gpio(void);

/** Configure PE8..PE15 as push-pull outputs for LEDs. */
void configure_beam_led_gpio(void);

// ---- Flexible Potentiometer GPIO ----
void enable_flexpot_clocks(void);
void configure_flexpot_gpio(void);


#endif // GPIO_H
