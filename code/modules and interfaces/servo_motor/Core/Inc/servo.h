/*
 * servo.h
 *
 *  Created on: May 14, 2025
 *      Author: shuvayu
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_

#include <stdint.h>
#include "stm32f303xc.h"

extern void (*on_button_press)();
void enable_clocks();
void enableGPIOAButton();
void enable_interrupt();

void rotate_servo_90();
void EXTI0_IRQHandler(void);

#endif /* INC_SERVO_H_ */
