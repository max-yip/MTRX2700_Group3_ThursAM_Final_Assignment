/*
 * colour_detector.h
 *
 *  Created on: May 20, 2025
 *      Author: maxyi
 */

#ifndef COLOUR_DETECTOR_H_
#define COLOUR_DETECTOR_H_

#include "serial.h"
#include "gpio.h"
#include "adc.h"


// State Machine
typedef enum {
    STATE_SET_RED, // set led to red
    STATE_READ_RED, // read ldr adc
    STATE_SET_GREEN, // set led to green
    STATE_READ_GREEN, // read ldr adc
    STATE_SET_BLUE, // set led to blue
    STATE_READ_BLUE, // read ldr adc
	STATE_CALIBRATION,
    STATE_DETERMINE_COLOUR, // determine colour
    STATE_TRANSMIT // output (currently its transmit through serial)
} ColorDetectState;

// colour type 1 = red, 2 = green
typedef enum {
    NO_COLOUR = 0,
    RED = 1,
    GREEN = 2
} ColourType;


extern ColourType detectedColour;


void handleStateMachine(void);

//void calibrateADC (uint16_t *red, uint16_t *green, uint16_t *blue);
//
//void readColourADC(uint16_t *red, uint16_t *green, uint16_t *blue);
//
//const uint8_t* determineColour(uint16_t red_init, uint16_t green_init, uint16_t blue_init,
//                         uint16_t red_adc, uint16_t green_adc, uint16_t blue_adc);
//
//void transmitColourData(const char *colour, uint16_t red, uint16_t green, uint16_t blue, SerialPort *serial_port);



#endif /* COLOUR_DETECTOR_H_ */
