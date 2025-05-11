#include "gpio.h"

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

#define BUTTONPRIORITY 2 // CAN BE CHANGED

void (*onButtonPress)() = 0x00;


// ---------- CLOCK ENABLE ----------


void enableGPIOClocks() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOEEN; // Enable GPIO A, C, and E
	return;
}


// ---------- GPIO CONFIG ----------


void enableGPIOAButton() {
	GPIOA->MODER &= ~(0x3 << (0 * 2));
	return;
}

void enableGPIOELEDS() {
	uint16_t *led_output_registers = ((uint16_t *)&(GPIOE->MODER)) + 1;
	*led_output_registers = 0x5555;
}


// ---------- SPECIFIC GPIO CONFIG ----------

// > LED ACCESS <

void setLEDState(uint8_t led_mask) {
	uint8_t *ODR = ((uint8_t*)&(GPIOE->ODR)) + 1;
	*ODR = led_mask;
	return;
}

uint8_t getLEDState() {
	uint8_t ODR = *(((uint8_t*)&(GPIOE->ODR)) + 1);
	return ODR;
}

void setButtonCallback(void (*cb)(void)) {
	onButtonPress = cb;
}

// ---------- INTERUPT ENABLE ----------

void enableButtonInterrupt() {
	// Disable the interrupts while messing around with the settings
	__disable_irq();

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable the system configuration controller (SYSCFG in RCC)
	SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PA; /*
	* External Interrupts details on large manual page 294)
	* PA0 is on interrupt EXTI0 large manual - page 250
	* EXTI0 in  SYSCFG_EXTICR1 needs to be 0x00 (SYSCFG_EXTICR1_EXTI0_PA)
	*/
	EXTI->RTSR |= EXTI_RTSR_TR0; // Select EXTI0 interrupt on rising edge
	EXTI->IMR |= EXTI_IMR_MR0; // set the interrupt from EXTI line 0 as 'not masked' - as in, enable it.
	NVIC_SetPriority(EXTI0_IRQn, BUTTONPRIORITY); // Set Priority
	NVIC_EnableIRQ(EXTI0_IRQn); // Tell the NVIC module that EXTI0 interrupts should be handled

	__enable_irq(); // Re-enable all interrupts (now that we are finished)
	return;
}

// ---------- INTERUPT HANDLER ----------

void EXTI0_IRQHandler() {
	// run the button press handler (make sure it is not null first !)
	if (onButtonPress) {
		onButtonPress();
	}

	EXTI->PR |= EXTI_PR_PR0; // reset the interrupt
	return;
}

// ---------- MISC ----------

void chaseLed(){
	uint8_t led_register = getLEDState();

	/* If nothing lit, seed the pattern with the lowest LED */
	if (led_register == 0) led_register = 0x01;

	/* Rotate the entire pattern one step to the left */
	led_register = (led_register << 1) | (led_register >> 7);

	/* Write the new pattern to the LED driver */
	setLEDState(led_register);
}

void pendingChaseLed(){
	uint8_t led_register = getLEDState();

	/* If nothing lit, seed the pattern with the lowest LED */
	if (led_register == 0) led_register = 0x01;

	/* Rotate the entire pattern one step to the left */
	led_register = (led_register << 1) | (led_register >> 7);

	/* Write the new pattern to the LED driver */
	set_pending_bit_mask(led_register);
}


// ---------- MAJOR INIT ----------


void init_main(void (*cb)(void)) {
	enableGPIOClocks();
	enableGPIOAButton();
	enableGPIOELEDS();
	enableButtonInterrupt();
	EXTI0_IRQHandler();
	setButtonCallback(cb);
}












