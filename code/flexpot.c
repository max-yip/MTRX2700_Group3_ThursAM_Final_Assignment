/*
 * flexpot.c
 *
 *  Created on: Apr 27, 2025
 *      Author: Paul Akle
 */
// flexpot.c
#include "flexpot.h"
#include "stm32f303xc.h"   // CMSIS

// below this raw ADC count we treat as “no touch”
#define NO_TOUCH_THRESHOLD   100U

void FlexPot_Init(void) {
    // enable GPIOC & ADC12
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN
                 | RCC_AHBENR_ADC12EN;

    // PC0 → analog
    GPIOC->MODER |= (3U << (0 * 2));

    // ADC clock = PCLK/1
    ADC12_COMMON->CCR = (ADC12_COMMON->CCR & ~ADC12_CCR_CKMODE)
                      |  ADC12_CCR_CKMODE_0;

    // power up ADC regulator
    ADC1->CR = (ADC1->CR & ~ADC_CR_ADVREGEN)
             |  ADC_CR_ADVREGEN_0;
    for (volatile int i = 0; i < 1000; i++);

    // self-calibrate single-ended
    ADC1->CR &= ~ADC_CR_ADCALDIF;
    ADC1->CR |=  ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL);

    // long sample time on IN6 (PC0)
    ADC1->SMPR1 &= ~(0x7U << ADC_SMPR1_SMP6_Pos);
    ADC1->SMPR1 |=  (0x7U << ADC_SMPR1_SMP6_Pos);

    // enable ADC1
    ADC1->ISR |= ADC_ISR_ADRDY;
    ADC1->CR  |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
}

uint8_t FlexPot_GetPosition(void) {
    // select channel 6 (PC0) for one conversion
    ADC1->SQR1 = (6U << ADC_SQR1_SQ1_Pos);

    // start & wait
    ADC1->CR |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC));

    uint32_t raw = ADC1->DR & 0x0FFF;  // 0..4095

    // if below threshold → no touch
    if (raw < NO_TOUCH_THRESHOLD) {
        return 0;
    }

    // map raw [0..4095] → pos [1..8]
    // multiply by 7 so the max raw (4095) gives 7, +1 → 8
    uint8_t pos = (raw * 7U) / 4095U + 1U;
    if (pos > 8U) pos = 8U;
    return pos;
}

