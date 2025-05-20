/*
 * colour_detector.c
 *
 *  Created on: May 20, 2025
 *      Author: maxyi
 */


#include "colour_detector.h"

static uint8_t calibrate = 0; // 0 = not calibrated, 1 = have calibrated
static uint16_t red_initial, green_initial, blue_initial;
static uint16_t red_adc, green_adc, blue_adc;
static uint8_t colour_serial[32];
static ColorDetectState currentState = STATE_SET_RED; // starting state
//static uint8_t debug_buffer[128]; // debugging purpose

ColourType detectedColour = NO_COLOUR;


// Calibration Helper Function
void save_readings_to_initial(void){
	red_initial = red_adc;
	green_initial = green_adc;
	blue_initial = blue_adc;
}


// State Machine
void handleStateMachine(void) {
    switch (currentState) {

    	// read Red LDR
        case STATE_SET_RED:
            setRed();
            currentState = STATE_READ_RED;
            break;

        case STATE_READ_RED:
        	red_adc = singleReadADC();
            currentState = STATE_SET_GREEN;
            break;

        // read Green LDR
        case STATE_SET_GREEN:
            setGreen();
            currentState = STATE_READ_GREEN;
            break;

        case STATE_READ_GREEN:
        	green_adc = singleReadADC();
            currentState = STATE_SET_BLUE;
            break;


        // read Blue LDR
        case STATE_SET_BLUE:
            setBlue();
            currentState = STATE_READ_BLUE;
            break;

        case STATE_READ_BLUE:
            blue_adc = singleReadADC();
            if (calibrate == 0) {
				currentState = STATE_CALIBRATION; // calibrate if not calibrated yet
			} else {
				currentState = STATE_DETERMINE_COLOUR;
			}
            break;


        case STATE_CALIBRATION:
            save_readings_to_initial();
            calibrate = 1;
            currentState = STATE_SET_RED; // restart state machine
            break;


        case STATE_DETERMINE_COLOUR: {
        	if ((green_initial - green_adc) > 400) {
				detectedColour = GREEN; // set colour to green
			}

        	else if ((red_initial - red_adc) > 400) {
				detectedColour = RED; // set colour to red
			}

        	else {
				detectedColour = NO_COLOUR; // set no colour
			}

            currentState = STATE_TRANSMIT; // or other output/loop back
            break;
        }


        // optional debug state
        case STATE_TRANSMIT:
            // Send over USART

        	if (detectedColour == RED){
        		sprintf((char*)colour_serial, "red detected!\r\n");
        	}
        	else if (detectedColour == GREEN){
        		sprintf((char*)colour_serial, "green detected!\r\n");
        	}
        	else {
        		sprintf((char*)colour_serial, "no colour detected!\r\n");
        	}
        	transmitString(colour_serial, &USART1_PORT);

//
//			sprintf((char*)debug_buffer, "red_val: %hu, green_val: %hu, blue_val: %hu\r\n", red_adc, green_adc, blue_adc);
//			transmitString(debug_buffer, &USART1_PORT);

            currentState = STATE_SET_RED; // loop back
            break;
    }
}



