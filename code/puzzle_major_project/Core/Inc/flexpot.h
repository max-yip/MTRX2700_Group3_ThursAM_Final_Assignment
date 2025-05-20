/*
 * flexpot.h
 *
 *  Created on: Apr 27, 2025
 *      Author: Paul Akle
 */

#ifndef FLEXPOT_H
#define FLEXPOT_H

#include <stdint.h>

/// Initialize ADC (clock, calibration, etc.)
void FlexPot_Init(void);

/// Read one pot on the given ADC channel (6 or 7), map 0..4095 → 1..6 (0=no‐touch)
uint8_t FlexPot_GetPosition(uint8_t channel);

#endif // FLEXPOT_H
