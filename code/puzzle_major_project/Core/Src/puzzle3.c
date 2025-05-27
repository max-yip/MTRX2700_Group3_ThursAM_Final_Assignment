/*
 * puzzle3.c
 *
 *  Created on: May 26, 2025
 *      Author: Paul Akle
 */

// puzzle3.c
// Module: Puzzle 3 - FlexPot coordinate game
// Polls two flexible potentiometers every 2 seconds, provides closer/further
// feedback over serial, and rotates a servo when (3,3) is found.

#include "puzzle3.h"
#include "flexpot.h"
#include "servo.h"
#include "serial.h"
#include "stm32f303xc.h"   // for USART1
#include <stdio.h>
#include <string.h>
#include <math.h>

extern SerialPort USART1_PORT;   // from serial.c

/*
busy-wait delay for ~2 seconds (tune constant as needed).
*/
static void Delay2Sec(void) {
    for (volatile uint32_t i = 0; i < 8000000; i++) {
        __asm__("nop");
    }
}

/*
Run the "find (3,3)" puzzle:
       - Polls X,Y from flex-pots every 2s
       - Prints coords and closer/further hints
       - On correct (3,3), rotates servo, announces success, then stops polling
*/
void Puzzle3_Run(void) {
    char buf[64];
    float prev_dist = INFINITY;

    for (;;) {
        Delay2Sec();

        uint8_t x = FlexPot_GetPosition(6);  // PC0/IN6
        uint8_t y = FlexPot_GetPosition(8);  // PC2/IN8

        // success case
        if (x == 3 && y == 3) {
            rotate_servo_90();
            snprintf(buf, sizeof(buf),
                     "Correct! Coordinates are 3,3\r\n");
            SerialOutputString((uint8_t*)buf, &USART1_PORT);
            // disable USART1 peripheral to stop further serial output
            USART1->CR1 &= ~USART_CR1_UE;
            break;  // exit polling loop
        }

        // print current coordinates
        snprintf(buf, sizeof(buf),
                 "X = %u, Y = %u\r\n", x, y);
        SerialOutputString((uint8_t*)buf, &USART1_PORT);

        // compute Euclidean distance to (3,3)
        float dx = (float)x - 3.0f;
        float dy = (float)y - 3.0f;
        float dist = sqrtf(dx*dx + dy*dy);

        // give closer/further feedback
        if (dist < prev_dist) {
            strcpy(buf, "Closer!\r\n");
        } else if (dist > prev_dist) {
            strcpy(buf, "Further...\r\n");
        } else {
            strcpy(buf, "Same distance.\r\n");
        }
        SerialOutputString((uint8_t*)buf, &USART1_PORT);

        prev_dist = dist;
    }
    // Polling stopped; function returns
}
