/*
 * servo.c
 *
 *  Created on: May 14, 2025
 *      Author: shuvayu
 */

// servo.c
// Module: Servo Control via TIM2 PWM on PA15
// Provides initialization of TIM2 for PWM output on PA15 and a helper
// to rotate the servo approximately 90 degrees by adjusting the duty cycle.

#include "stm32f303xc.h"
#include "servo.h"

// prototypes for our GPIO helpers (defined in gpio.c)
extern void enable_servo_clocks(void);
extern void configure_servo_gpio(void);

/*
Initialise TIM2 for PWM-based servo control on PA15 (TIM2_CH1).

Steps:
   1) Enable GPIOA and TIM2 clocks.
   2) Configure PA15 as AF1 (TIM2_CH1) output.
   3) Set prescaler and auto-reload for a 20 ms period (50 Hz).
   4) Configure channel 1 in PWM Mode 1 with preload enabled.
   5) Enable the channel 1 output.
   6) Enable auto-reload preload and start the timer.
   7) Set an initial 1.0 ms pulse width (servo 0° position).
*/
void Servo_Init(void) {
    // 1) Enable GPIOA & TIM2 clocks
    enable_servo_clocks();

    // 2) Configure PA15 for TIM2_CH1 alternate function
    configure_servo_gpio();

    // 3) Timer2 base: 48 MHz / (PSC+1) = 1 MHz timer clock
    TIM2->PSC = 47;              // prescaler
    TIM2->ARR = 20000 - 1;       // 20 ms period (50 Hz)
    TIM2->EGR = TIM_EGR_UG;      // force immediate update

    // 4) PWM mode 1 on CH1, enable preload
    TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S
                   | TIM_CCMR1_OC1M
                   | TIM_CCMR1_OC1PE);
    TIM2->CCMR1 |=  (6U << TIM_CCMR1_OC1M_Pos)  // OC1M = 110 (PWM1)
                  | TIM_CCMR1_OC1PE;            // preload enable

    // 5) Enable channel 1 output
    TIM2->CCER |= TIM_CCER_CC1E;

    // 6) Enable auto-reload preload and start timer
    TIM2->CR1 |= TIM_CR1_ARPE
               | TIM_CR1_CEN;

    // 7) Set default 1.0 ms pulse width (0°)
    TIM2->CCR1 = 1000;
}

/*
- Rotate the servo to approximately 90 degrees.
- Writes a 1.8 ms pulse width to CCR1, corresponding to ~90°.
*/
void rotate_servo_90(void) {
    TIM2->CCR1 = 1800;
}
