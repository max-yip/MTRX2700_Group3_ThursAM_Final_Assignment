/*
 * led_sequence.h
 *
 *  Created on: May 23, 2025
 *      Author: maxyi
 */

#ifndef LED_SEQUENCE_H_
#define LED_SEQUENCE_H_

#include "gpio.h"
#include "timer.h"
#include "serial.h"
#include "colour_detector.h"

#define LED_COUNT 7

// state checking when the leds are on or off
typedef enum {
	STATE_ON,
	STATE_OFF
} LedState;


void setLEDs(void);


#endif /* LED_SEQUENCE_H_ */
