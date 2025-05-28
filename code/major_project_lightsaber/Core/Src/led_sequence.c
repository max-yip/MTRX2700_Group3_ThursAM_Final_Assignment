/*
 * led_sequence.c
 *
 *  Created on: May 23, 2025
 *      Author: maxyi
 */

#include "led_sequence.h"



// Current state of the LED sequence (either turning ON or OFF)
volatile LedState curr_led_state = STATE_OFF;

// Flag indicating if a sequence is currently running
volatile uint8_t sequence_in_progress = 0;

// Index of the currently addressed LED
volatile uint8_t current_led = 0;

// Bitmask representing the current LED ON/OFF pattern
volatile uint8_t bitmask = 0;

// Debug message buffer for serial prints
static uint8_t debug_buffer[128];

// LED pin mappings (assuming GPIO pin numbers or indexes)
const uint8_t led_pins[] = {0, 1, 2, 3, 4, 5, 6};


// ===================== //
//     Main LED Logic    //
// ===================== //
void setLEDs(void) {
	switch (curr_led_state) {

		// ------------------------------ //
		// STATE_OFF: Turning LEDs ON (0 → 6)
		// ------------------------------ //
		case STATE_OFF:
			if (detectedColour == GREEN) {
				// Shift bitmask left and set LSB to 1 (adds next LED ON)
				bitmask = (bitmask << 1) | 1;

				// Update LED states based on new bitmask
				setLEDState(bitmask);

//				sprintf((char*)debug_buffer, "TURNING LEDS ON %d\r\n", bitmask);
//				transmitString(debug_buffer, &USART1_PORT);

				// Move to next LED in sequence
				current_led++;

				// If not finished, trigger next step after delay
				if (current_led < LED_COUNT) {
					startOneShotTIM3(50, setLEDs);  // 50 ms delay
				} else {
					// Sequence finished; switch to next state
					curr_led_state = STATE_ON;
					current_led = LED_COUNT - 1;  // Start from last LED
				}
				break;
			}
			break;

		// ------------------------------- //
		// STATE_ON: Turning LEDs OFF (6 → 0)
		// ------------------------------- //
		case STATE_ON:
			// Shift bitmask right (turns off highest LED)
			bitmask = (bitmask >> 1);

			// Update LED states
			setLEDState(bitmask);

//			sprintf((char*)debug_buffer, "TURNING LEDS OFF %d\r\n", bitmask);
//			transmitString(debug_buffer, &USART1_PORT);

			// Move backward to previous LED
			current_led--;

			// If not finished, continue sequence
			if ((int8_t)current_led >= -1) {
				startOneShotTIM3(50, setLEDs);  // 50 ms delay
			} else {
				// Sequence complete; reset state and flags
				curr_led_state = STATE_OFF;
				sequence_in_progress = 0;
				current_led = 0;
			}
			break;

		// ------------------------------- //
		// Default: Do nothing (fallback)
		// ------------------------------- //
		default:
			break;
	}
}
