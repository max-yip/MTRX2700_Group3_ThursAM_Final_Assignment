#ifndef TIMER_H
#define TIMER_H

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


// ---------- CLOCK ENABLE ----------


void enableTIMERClocks();


// ---------- INITIALISE TIMERS ----------


void initialiseTIM2(uint32_t);

void setTIM2callback(void (*)(void));

void setTIM3callback(void (*callback)(void));

// ---------- SPECIFC TIMER FUNCTIONALITY ---------

uint32_t getTIM2Currentms(void);

void setTIM2Currentms(uint32_t ms);

void resetTIM2(uint32_t ms);

void startOneShotTIM3(uint32_t ms, void (*callback)(void));

// ---------- INTERUPT ENABLE ----------


void enableTimerInterrupt();


// ---------- INTERUPT HANDLER ----------


void TIM2_IRQHandler();

void TIM3_IRQHandler();


// ---------- MISC ----------

// > setter for prv var <
void set_pending_bit_mask(uint8_t);

void updatePending();

void printBuffer();

void invertLEDs();


// ---------- MAJOR INIT ----------


void init_TIM2(uint32_t);


#endif
