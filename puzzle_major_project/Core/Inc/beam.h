/*
 * beam.h
 *
 *  Created on: Apr 25, 2025
 *      Author: Paul Akle
 */

// beam.h
#ifndef BEAM_H
#define BEAM_H

#include <stdint.h>
#include "stm32f3xx_hal.h"

// call once at startup to configure the beam sensor EXTI
void Beam_Init(void);

// returns how many times the beam has been broken (0…8)
uint8_t Beam_GetCount(void);

// resets the beam count back to zero and turns off all LEDs
void Beam_ResetCount(void);

#endif // BEAM_H
