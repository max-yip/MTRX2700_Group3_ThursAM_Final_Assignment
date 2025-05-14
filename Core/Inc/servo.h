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
void rotate_servo_90();
void EXTI0_IRQHandler(void);
void TIM2_IRQHandler(void);

void enable_interrupt();
void trigger_prescaler(uint8_t prescaler, TIM_TypeDef *TIMER);


#endif /* INC_SERVO_H_ */
