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

// ------ LEDRegister from wk 9 -----

typedef union {
	uint8_t all_leds;
	struct {
		uint8_t led_pair_1 : 2;
		uint8_t led_pair_2 : 2;
		uint8_t led_set_of_4 : 4;
	} led_groups;
} LedRegister;


typedef struct {
	uint8_t var1;
	uint32_t var2;
	uint8_t var3;
	uint32_t var4;
	uint8_t var5;
	uint8_t var6;
} struct_1;


typedef struct {
	uint32_t var2;
	uint32_t var4;
	uint8_t var1;
	uint8_t var3;
	uint8_t var5;
	uint8_t var6;
} struct_2;


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
