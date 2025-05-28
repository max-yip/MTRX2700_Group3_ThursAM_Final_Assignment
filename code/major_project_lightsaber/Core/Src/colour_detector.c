/*
 * colour_detector.c
 *
 *  Created on: May 20, 2025
 *      Author: maxyip
 */


#include "colour_detector.h"

// Calibration state flag: 0 = not calibrated, 1 = calibrated
static uint8_t calibrate = 0;

// Initial ADC values recorded during calibration
static uint16_t red_initial, green_initial, blue_initial;

// Current ADC readings for each color
static uint16_t red_adc, green_adc, blue_adc;

// Buffer to store the output string displayed on LCD
static uint8_t colour_serial[32];

// Current state of the color detection state machine
static ColorDetectState currentState = STATE_SET_RED;

// Debugging buffer used for serial UART output
static uint8_t debug_buffer[128];

// Output result of color detection
ColourType detectedColour = NO_COLOUR;


// ============================== //
//  Calibration Helper Function  //
// ============================== //
void save_readings_to_initial(void){
    // Store current readings as reference for future comparisons
    red_initial = red_adc;
    green_initial = green_adc;
    blue_initial = blue_adc;
}


// ========================== //
//   Main Color State Machine //
// ========================== //
void handleStateMachine(void) {
    switch (currentState) {

        // ----------- //
        // RED Channel //
        // ----------- //
        case STATE_SET_RED:
            setRed();                       // Enable red filter (e.g. select red LED or LDR channel)
            currentState = STATE_READ_RED; // Next: read red value
            break;

        case STATE_READ_RED:
            red_adc = singleReadADC();     // Perform ADC read for red
            currentState = STATE_SET_GREEN; // Move to green
            break;

        // ------------ //
        // GREEN Channel //
        // ------------ //
        case STATE_SET_GREEN:
            setGreen();                    // Enable green filter
            currentState = STATE_READ_GREEN;
            break;

        case STATE_READ_GREEN:
            green_adc = singleReadADC();   // Perform ADC read for green
            currentState = STATE_SET_BLUE;
            break;

        // ----------- //
        // BLUE Channel //
        // ----------- //
        case STATE_SET_BLUE:
            setBlue();                     // Enable blue filter
            currentState = STATE_READ_BLUE;
            break;

        case STATE_READ_BLUE:
            blue_adc = singleReadADC();    // Perform ADC read for blue

            // If not yet calibrated, proceed to calibration
            if (calibrate == 0) {
                currentState = STATE_CALIBRATION;
            } else {
                currentState = STATE_DETERMINE_COLOUR;
            }
            break;

        // ------------------- //
        // Calibration Routine //
        // ------------------- //
        case STATE_CALIBRATION:
            save_readings_to_initial();    // Save current readings as baseline
            calibrate = 1;                 // Mark system as calibrated
            currentState = STATE_SET_RED;  // Restart measurement cycle
            break;

        // ------------------- //
        // Colour Determination //
        // ------------------- //
        case STATE_DETERMINE_COLOUR: {
            // Check for green dominance based on drop from calibrated value
            if ((green_initial - green_adc) > 400) {
                detectedColour = GREEN;
            }
            // Check for red dominance
            else if ((red_initial - red_adc) > 400) {
                detectedColour = RED;
            }
            // If no significant drop, no colour detected
            else {
                detectedColour = NO_COLOUR;
            }

            currentState = STATE_TRANSMIT; // Move to output state
            break;
        }

        // ---------------------- //
        // Transmit / Output State //
        // ---------------------- //
        case STATE_TRANSMIT:
            // Debug print of all ADC values over serial
            sprintf((char*)debug_buffer, "red_val: %hu, green_val: %hu, blue_val: %hu\r\n", red_adc, green_adc, blue_adc);
            transmitString(debug_buffer, &USART1_PORT);

            // Set message depending on detected colour
            if (detectedColour == RED) {
                sprintf((char*)colour_serial, "A Sith Lord?"); // Red message
            } else if (detectedColour == GREEN) {
                sprintf((char*)colour_serial, "GREEN LIGHTSABER!"); // Green message
            } else {
                // No valid colour — just clear and reset
                HD44780_Clear();
                currentState = STATE_SET_RED;
                break;
            }

            // Display message on LCD
            HD44780_Clear();                                  // Clear screen
            HD44780_SetCursor(0, 0);                          // Position at top-left
            HD44780_PrintStr((const char*)colour_serial);     // Display result

            // Restart cycle
            currentState = STATE_SET_RED;
            break;
    }
}
