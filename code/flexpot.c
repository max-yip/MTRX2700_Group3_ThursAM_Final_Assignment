/*
 * flexpot.c
 *
 *  Created on: Apr 27, 2025
 *      Author: Paul Akle
 */
#include "flexpot.h"
#include "stm32f303xc.h"   // CMSIS

#define NO_TOUCH_THRESHOLD   100U
#define FLEXPOT_STEPS        6U   // we want positions 1..6

void FlexPot_Init(void) {
    // 1) Enable GPIOC & ADC12 clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN
                 | RCC_AHBENR_ADC12EN;

    // 2) PC0 & PC2 → analog
    GPIOC->MODER |= (3U << (0*2))      // PC0 = IN6
                  | (3U << (2*2));     // PC2 = IN8

    // 3) ADC clock = PCLK/1
    ADC12_COMMON->CCR = (ADC12_COMMON->CCR & ~ADC12_CCR_CKMODE)
                      | ADC12_CCR_CKMODE_0;

    // 4) Power up ADC regulator & delay
    ADC1->CR = (ADC1->CR & ~ADC_CR_ADVREGEN)
             | ADC_CR_ADVREGEN_0;
    for (volatile int i = 0; i < 1000; i++);

    // 5) Self‐calibrate
    ADC1->CR &= ~ADC_CR_ADCALDIF;
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL);

    // 6) Long sample time on IN6 (PC0) & IN8 (PC2)
    ADC1->SMPR1 = (0x7U << ADC_SMPR1_SMP6_Pos)   // channel 6
                | (0x7U << ADC_SMPR1_SMP8_Pos);  // channel 8

    // 7) Enable ADC & wait for ready
    ADC1->ISR |= ADC_ISR_ADRDY;
    ADC1->CR  |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
}

/// Poll one channel once, map raw→1..6
uint8_t FlexPot_GetPosition(uint8_t channel) {
    // 1) Select channel for single conversion
    ADC1->SQR1 = ((uint32_t)channel << ADC_SQR1_SQ1_Pos);

    // 2) Start conversion
    ADC1->CR |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC));

    // 3) Read & clear
    uint32_t raw = ADC1->DR & 0x0FFF;

    // 4) No‐touch?
    if (raw < NO_TOUCH_THRESHOLD) {
        return 0;
    }

    // 5) Map 0..4095 → 1..FLEXPOT_STEPS
    uint8_t pos = (raw * (FLEXPOT_STEPS - 1U)) / 4095U + 1U;
    if (pos > FLEXPOT_STEPS) pos = FLEXPOT_STEPS;
    return pos;
}
