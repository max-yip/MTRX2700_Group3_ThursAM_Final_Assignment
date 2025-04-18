#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>


/* layout of file:
 *
 * - Definitions
 * - GPIO clock enable
 * - GPIO configuration
 * - configuring specfic GPIO functions
 * 		- Specific register set ups
 * - Initialise Interrupts
 * - Initialise Interrupt Handler
 * - MISC functionality
 * - Major Initialise
 */

// ---------- CLOCK ENABLE ----------


void enableGPIOClocks();


// ---------- GPIO CONFIG ----------


void enableGPIOAButton();

void enableGPIOELEDS();

void setButtonCallback(void (*)(void));


// ---------- SPECIFIC GPIO CONFIG ----------

// > LED ACCESS <

void setLEDState(uint8_t);

uint8_t getLEDState();


// ---------- INTERUPT ENABLE ----------


void enableButtonInterrupt();


// ---------- INTERUPT HANDLER ----------


void EXTI0_IRQHandler();


// ---------- MISC ----------


void chaseLed();

void pendingChaseLed();


// ---------- MAJOR INIT ----------


void init_main(void (*)(void));


#endif
