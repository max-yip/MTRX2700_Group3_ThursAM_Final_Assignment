#include <stdint.h>
#include "stm32f303xc.h"
#include "gpio.h"
#include "serial.h"

/* layout of file:
 *
 * - Definitions
 * - Timer enable
 * - Initialise timers
 * - Initialise Specfic timer functions
 * 		- Specific register set ups
 * - Initialise Interrupts
 * - Initialise Interrupt Handler
 * - MISC functionality
 * - Major Initialise
 */


#define CLOCKSPEED 72000000 // 72 MHz
#define TIM2PRIORITY 2 // CAN BE CHANGED
#define TIM3PRIORITY 5

volatile uint32_t TIM2_CURRENT_MS;

void (*TIM2HandlerFunction)() = 0x00;
void (*TIM3HandlerFunction)() = 0x00;

// ---------- CLOCK ENABLE ----------

void enableTIMERClocks() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enale clock to timer 2
	// RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable  system configuration controller for interrupts
	return;
}

// ---------- INITIALISE TIMERS ----------

void initialiseTIM2(uint32_t ms){
	TIM2_CURRENT_MS = ms;
	TIM2->PSC = CLOCKSPEED / 1000 - 1; // PreSCaler: update timer clock whenever we get to the PSC amount
	TIM2->ARR = ms - 1; // Auto-Reload Register: Fires an interrupt every time you count up to it
	TIM2->CR1 |= TIM_CR1_CEN; // Begin the timer
	return;
}

void setTIM2callback(void (*callback)(void)) {
    TIM2HandlerFunction = callback;
    return;
}

void setTIM3callback(void (*callback)(void)) {
    TIM3HandlerFunction = callback;
    return;
}

// ---------- SPECIFC TIMER FUNCTIONALITY ---------


uint32_t getTIM2Currentms(void) {
	return TIM2_CURRENT_MS;
}

void setTIM2Currentms(uint32_t ms) {
	TIM2_CURRENT_MS = ms;
	if (TIM2) {
		TIM2->ARR = ms - 1;
	}
}

void resetTIM2(uint32_t ms){
	TIM2->CNT = 0;
	if (ms) {
		TIM2->ARR = ms - 1;
	}
}

void startOneShotTIM3(uint32_t ms, void (*callback)(void)){
	TIM3HandlerFunction = callback;

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	TIM3->PSC = (CLOCKSPEED / 1000) - 1;
	TIM3->ARR = ms - 1;
	TIM3->CNT = 0;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 |= TIM_CR1_CEN;

	NVIC_SetPriority(TIM3_IRQn, TIM3PRIORITY);
	NVIC_EnableIRQ(TIM3_IRQn);
}

// ---------- INTERUPT ENABLE ----------

void enableTimerInterrupt(){
	// Disable the interrupts while messing around with the settings
	__disable_irq();

	TIM2->DIER |= TIM_DIER_UIE; // Activates the overflow interupt when counting beyond ARR

	NVIC_EnableIRQ(TIM2_IRQn); // Enable the interupt in NVIC (Nested Vectored Interrupt Controller)
	NVIC_SetPriority(TIM2_IRQn, TIM2PRIORITY);  // Priority set

	__enable_irq(); // Re-enable all interrupts (now that we are finished)
	return;
}

// ---------- INTERUPT HANDLER ----------

void TIM2_IRQHandler() {
    if (TIM2->SR & TIM_SR_UIF) {        // Check update flag
        TIM2->SR &= ~TIM_SR_UIF;        // Clear flag

        if (TIM2HandlerFunction) {
        	TIM2HandlerFunction();
        }
    }
    return;
}

void TIM3_IRQHandler() {
	if (TIM3->SR & TIM_SR_UIF) {
		TIM3->SR &= ~TIM_SR_UIF;

		// Disable the TIM3 counter by clearing the CEN (Counter ENable)
		TIM3->CR1 &= ~TIM_CR1_CEN;

		// Disable the TIM3 update interrupt by clearing the UIE (Update Interrupt Enable)
		TIM3->DIER &= ~TIM_DIER_UIE;


		if (TIM3HandlerFunction){
			TIM3HandlerFunction();
		}
	}
}


// ---------- MISC ----------


uint8_t pending_bit_mask = 0x00;


void set_pending_bit_mask(uint8_t mask) {
	pending_bit_mask = mask;
	return;
}

void updatePending(){
	uint8_t current = getLEDState();

	if (pending_bit_mask != current) {
		setLEDState(pending_bit_mask);
	}
}

void printBuffer(){
	transmitString("HELLO FROM timer.c", &USART1_PORT);
}

void invertLEDs() {
	setLEDState(~getLEDState());

}



// ---------- MAJOR INIT ----------


void init_TIM2(uint32_t ms) {
	enableTIMERClocks();
	initialiseTIM2(ms);
	enableTimerInterrupt();
	TIM2_IRQHandler();
}

















