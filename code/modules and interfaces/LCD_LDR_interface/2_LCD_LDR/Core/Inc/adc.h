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

void initialiseSingleADC(void);

uint16_t readLDR_ADC(void);

#endif /* ADC_H_ */
