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



void rotate_servo_90(){
		TIM2->CCR1 = 1800;
		TIM2->CCR2 = 1800;


	}



