/*
 * led_sequence.c
 *
 *  Created on: May 23, 2025
 *      Author: maxyi
 */

#include "led_sequence.h"


volatile LedState curr_led_state = STATE_OFF;
volatile uint8_t sequence_in_progress = 0;
volatile uint8_t current_led = 0;
volatile uint8_t bitmask = 0;

static uint8_t debug_buffer[128];

const uint8_t led_pins[] = {0,1,2,3,4,5,6};


void setLEDs(void) {
	switch (curr_led_state) {
		case STATE_OFF:  // Turning LEDs ON from 0 → 6
			if (detectedColour == GREEN){
				// Turn ON current LED
				bitmask = (bitmask << 1) | 1;
//				sprintf((char*)debug_buffer, "TURNING LEDS ON %d\r\n", bitmask);
//				transmitString(debug_buffer, &USART1_PORT);
				setLEDState(bitmask);

				current_led++;
				if (current_led < LED_COUNT) {
					startOneShotTIM3(50, setLEDs);
				} else {
					curr_led_state = STATE_ON;
					current_led = LED_COUNT - 1;
				}
				break;
			}
			break;

		case STATE_ON:  // Turning LEDs OFF from 6 → 0
			// Turn OFF current LED
			bitmask = (bitmask >> 1);
//			sprintf((char*)debug_buffer, "TURNING LEDS OFF %d\r\n", bitmask);
//			transmitString(debug_buffer, &USART1_PORT);
			setLEDState(bitmask);

			current_led--;
			if ((int8_t)current_led >= -1) {
				startOneShotTIM3(50, setLEDs);
			} else {
				curr_led_state = STATE_OFF;
				sequence_in_progress = 0;  // Sequence complete
				current_led = 0;
			}
			break;

		default:
			break;
	}
}

