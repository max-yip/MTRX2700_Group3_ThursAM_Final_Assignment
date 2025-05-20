/*
 * adc.h
 *
 *  Created on: May 19, 2025
 *      Author: maxyi
 */

#ifndef ADC_H_
#define ADC_H_
#include <stdint.h>
#include "stm32f303xc.h"

void initialiseSingleADC();

uint16_t singleReadADC();

void continuousReadSingleChannelADC();

void SingleReadMultiChannelADC();

#endif /* ADC_H_ */
