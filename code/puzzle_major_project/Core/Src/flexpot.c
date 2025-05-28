/*
 * flexpot.c
 *
 *  Created on: Apr 27, 2025
 *      Author: Paul Akle
 */

// flexpot.c
// Module: Flexible Potentiometer Reader
// Provides initialisation and single-shot ADC readings on two flex-pots
// mapped to discrete positions 1..6 (0 = no touch).

#include "flexpot.h"
#include "gpio.h"              // GPIO code
#include "stm32f303xc.h"       // CMSIS

#define NO_TOUCH_THRESHOLD   100U  // raw ADC count threshold
#define FLEXPOT_STEPS        6U    // map raw to 1..6 positions

/*
Initialise ADC1 to read from PC0 (IN6) & PC2 (IN8).
Utilises GPIO helpers for clock and pin setup.
*/
void FlexPot_Init(void) {
    // 1) GPIO setup
    enable_flexpot_clocks();      // RCC clocks
    configure_flexpot_gpio();     // PC0/PC2 → analog

    // 2) ADC common clock = PCLK/1
    ADC12_COMMON->CCR = (ADC12_COMMON->CCR & ~ADC12_CCR_CKMODE)
                      | ADC12_CCR_CKMODE_0;

    // 3) Power up ADC regulator & small delay
    ADC1->CR = (ADC1->CR & ~ADC_CR_ADVREGEN)
             | ADC_CR_ADVREGEN_0;
    for (volatile int i = 0; i < 1000; i++);

    // 4) Self-calibrate single-ended mode
    ADC1->CR &= ~ADC_CR_ADCALDIF;
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL);

    // 5) Long sample time on channels 6 & 8
    ADC1->SMPR1 = (0x7U << ADC_SMPR1_SMP6_Pos)
                | (0x7U << ADC_SMPR1_SMP8_Pos);

    // 6) Enable ADC1 and wait until ready
    ADC1->ISR |= ADC_ISR_ADRDY;
    ADC1->CR  |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
}

/*
- Perform one ADC conversion on channel, map raw to position 1..6.
- ADC channel (6 for PC0, 8 for PC2)
- pos - 0=no-touch or 1..6 mapped position
*/
uint8_t FlexPot_GetPosition(uint8_t channel) {
    // start conversion on specified channel
    ADC1->SQR1 = ((uint32_t)channel << ADC_SQR1_SQ1_Pos);
    ADC1->CR  |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC));

    // read raw 12-bit value
    uint32_t raw = ADC1->DR & 0x0FFF;

    // no-touch check
    if (raw < NO_TOUCH_THRESHOLD) return 0;

    // map raw → 1..FLEXPOT_STEPS
    uint8_t pos = (uint8_t)((raw * (FLEXPOT_STEPS - 1U)) / 4095U + 1U);
    return (pos > FLEXPOT_STEPS) ? FLEXPOT_STEPS : pos;
}

