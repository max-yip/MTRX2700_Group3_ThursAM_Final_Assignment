/*
 * adc.c
 *
 *  Created on: May 18, 2025
 *      Author: siya
 */

#include "adc.h"

void initialiseSingleADC(void) {
    // Enable ADC12 clock
    RCC->AHBENR |= RCC_AHBENR_ADC12EN;

    // Synchronise ADC clock with system clock (HCLK/1)
    ADC12_COMMON->CCR |= ADC12_CCR_CKMODE_0;

    // Disable ADC and voltage regulator before configuration
    ADC1->CR &= ~ADC_CR_ADVREGEN;
    ADC1->CR |= ADC_CR_ADVREGEN_0;       // Enable ADC voltage regulator (01)
    ADC1->CR &= ~ADC_CR_ADCALDIF;        // Use single-ended mode

    // Calibrate the ADC
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL);     // Wait for calibration to finish

    // Configure the ADC to convert channel 2 (PA1), single conversion
    ADC1->SQR1 = 0;
    ADC1->SQR1 |= 0x02 << ADC_SQR1_SQ1_Pos;  // Channel 2 = PA1
    ADC1->SQR1 |= 0x00 << ADC_SQR1_L_Pos;    // 1 conversion

    // Single conversion mode (not continuous)
    ADC1->CFGR &= ~ADC_CFGR_CONT;

    // Enable ADC and wait until it's ready
    ADC1->CR |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
}

uint16_t readLDR_ADC(void) {
    // Start ADC conversion
    ADC1->CR |= ADC_CR_ADSTART;

    // Wait for conversion to complete
    while (!(ADC1->ISR & ADC_ISR_EOC));

    // Read the converted value
    return ADC1->DR;
}
