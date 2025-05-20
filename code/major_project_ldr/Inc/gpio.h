#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "stm32f303xc.h"
//#include "timer.h"

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

void enableGPIODLEDS();

void setButtonCallback(void (*)(void));


// ---------- SPECIFIC GPIO E CONFIG ----------

// > LED ACCESS <

void setLEDState(uint8_t);

uint8_t getLEDState();

// ---------- SPECIFIC GPIO D CONFIG ----------

void setRed();

void setGreen();

void setBlue();


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
