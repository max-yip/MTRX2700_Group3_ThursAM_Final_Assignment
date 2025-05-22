/*
 * servo.c
 *
 *  Created on: May 14, 2025
 *      Author: shuvayu
 */

#include <stdint.h>
#include "stm32f303xc.h"
#include "servo.h"



// store a pointer to the function that is called when a button is pressed
// set a default value of NULL so that it won't be called until the
// function pointer is defined
void (*on_button_press)() = 0x00;


// enable the clocks for desired peripherals (GPIOA, C and E)
void enable_clocks() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOEEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;     // <<< ENABLE TIM2 CLOCK
}


void enableGPIOAButton() {
	GPIOA->MODER &= ~(0x3 << (0 * 2));
	return;
}



void enable_interrupt() {
// Disable the interrupts while messing around with the settings
// otherwise can lead to strange behaviour
__disable_irq();

// Enable the system configuration controller (SYSCFG in RCC)
RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

// This tells PA0 (button) that it will be the cause of the interrupt
SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PA;

// Select EXTI0 interrupt on rising edge
EXTI->RTSR |= EXTI_RTSR_TR0; // rising edge of EXTI line 0 (includes PA0)

// set the interrupt from EXTI line 0 as 'not masked' by setting it to 1 - as in, enable it.
EXTI->IMR |= EXTI_IMR_MR0;

// Tell the NVIC module that EXTI0 interrupts should be handled
NVIC_SetPriority(EXTI0_IRQn, 3); // set priority
NVIC_EnableIRQ(EXTI0_IRQn);

// Re-enable all interrupts (now that we are finished)
__enable_irq();
}


void EXTI0_IRQHandler(void) {
// run the button press handler (make sure it is not null first !)
if (on_button_press != 0x00) {
on_button_press(); //  run callback function (rotate_servo_90)
}
// reset the interrupt (so it doesn't keep firing until the next trigger)
EXTI->PR |= EXTI_PR_PR0;

}



//void Servo_Init(void) {
//    enable_clocks();
//
//    // PA0→TIM2_CH1, PA1→TIM2_CH2, AF1
//    GPIOA->MODER &= ~((3U<<0)|(3U<<2));
//    GPIOA->MODER |=  ((2U<<0)|(2U<<2));
//    GPIOA->AFR[0] &= ~((0xFU<<0)|(0xFU<<4));
//    GPIOA->AFR[0] |=  ((1U<<0)|(1U<<4));
//
//    // Timer2 @1 MHz → 20 ms period → 50 Hz
//    TIM2->PSC = 48 - 1;
//    TIM2->ARR = 20000 - 1;
//
//    // PWM Mode 1 on CH1 & CH2, enable preload
//    TIM2->CCMR1  = (6U<<4)  | TIM_CCMR1_OC1PE
//                 | (6U<<12) | TIM_CCMR1_OC2PE;
//    TIM2->CCER  |= TIM_CCER_CC1E | TIM_CCER_CC2E;
//
//    // centre at 1.5 ms
//    TIM2->CCR1   = 1500;
//    TIM2->CCR2   = 1500;
//
//    // start timer
//    TIM2->CR1   |= TIM_CR1_ARPE | TIM_CR1_CEN;
//}

void rotate_servo_90(void) {
    TIM2->CCR1 = 1800;   // 1.8 ms pulse ≈ 90°
    TIM2->CCR2 = 1800;
}
