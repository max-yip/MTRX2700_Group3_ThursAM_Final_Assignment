/*
 * servo.c
 *
 *  Created on: May 14, 2025
 *      Author: shuvayu
 */

#include <stdint.h>
#include "stm32f303xc.h"
#include "servo.h"


volatile uint8_t move_servo = 0; // acts as flag such that servo only moves when flag is raised (move_servo==1)


// store a pointer to the function that is called when a button is pressed
// set a default value of NULL so that it won't be called until the
// function pointer is defined
void (*on_button_press)() = 0x00;


void trigger_prescaler(uint8_t prescaler, TIM_TypeDef *TIMER){
TIMER->PSC = prescaler;
TIMER->ARR |= 0x01; // set ARR to 1 (quick overflow to get it started)
TIMER->CNT |= 0x00; // reset counter
asm("NOP");
asm("NOP");
asm("NOP");
asm("NOP");
TIMER->ARR |= 0xFFFFFFFF; // set ARR back to default value (slow overflow)
TIMER->CR1 |= TIM_CR1_CEN; // start the counter counting

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

RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_TIM2EN; // power APB1 and timer 2

TIM2->DIER |= TIM_DIER_CC2IE;

NVIC_SetPriority(TIM2_IRQn, 2); // Set Priority
NVIC_EnableIRQ(TIM2_IRQn);

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


void TIM2_IRQHandler(void){
// check if timer hit the target value (dependent on psc value)
if (TIM2->SR & TIM_SR_CC2IF) {
move_servo = 1; // raise flag to allow servo to rotate
TIM2->SR &= ~TIM_SR_CC2IF; // Clear the CC2IF flag
	}
}


void rotate_servo_90(){
	// if flag is raised rotate the servo 90 degrees
	if (move_servo) {
		TIM2->CCR1 = 1500; // 90 degrees 1.5ms PWM

	move_servo = 0; // flag is back down after the servo has rotated 90 degrees
	}
}


