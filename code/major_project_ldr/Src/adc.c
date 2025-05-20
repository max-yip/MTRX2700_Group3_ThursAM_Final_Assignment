/*
 * adc.c
 *
 *  Created on: May 19, 2025
 *      Author: maxyi
 */
#include "adc.h"

void initialiseSingleADC(){

	// enable the clock for ADC1
	RCC->AHBENR |= RCC_AHBENR_ADC12EN;

	// set to synchronise the ADC with the clock
	ADC12_COMMON->CCR |= ADC12_CCR_CKMODE_0;

	// ADEN must be = 0 for configuration (is the default)
	ADC1->CR &= ~ADC_CR_ADVREGEN; // clear voltage regulator enable
	ADC1->CR |= ADC_CR_ADVREGEN_0; // set ADVREGEN TO 01
	ADC1->CR &= ~ADC_CR_ADCALDIF; // clear bit to enable Single-ended-input

	// calibrate the ADC (self calibration routine)
	ADC1->CR |= ADC_CR_ADCAL;
	while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL); // Waiting for the calibration to finish

	// select the channel, only one conversion so it goes in
	//  the first conversion slot (SQ1)
	//  and the L value should be 0 (L=0000 does one conversion only)
	ADC1->SQR1 = 0; // clear any existing channel requests
	ADC1->SQR1 |= 0x02 << ADC_SQR1_SQ1_Pos; // request channel 2 (PA1)
	ADC1->SQR1 |= 0x00 << ADC_SQR1_L_Pos; // set the number of channels to read

	// single shot mode
	ADC1->CFGR &= ~ADC_CFGR_CONT;

	// Enable the ADC
	ADC1->CR |= ADC_CR_ADEN;

	// Wait the ADC to be ready.
	while (ADC1->ISR == 0);

	return;
}

uint16_t singleReadADC(){
	uint16_t value = 0;
	uint32_t total = 0;

	// Take 8 samples
	for (int i = 0; i < 8; i++) {
		// Start conversion
		ADC1->CR |= ADC_CR_ADSTART;

		// Wait for end of conversion
		while ((ADC1->ISR & ADC_ISR_EOC) == 0);

		value = ADC1->DR;
		total += value;
	}

	uint16_t average = total / 8;

	return average;
}


// Example Code

void continuousReadSingleChannelADC() {

	// get a pointer to the location of the LEDs
	uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;

	// enable the clock for ADC1
	RCC->AHBENR |= RCC_AHBENR_ADC12EN;

	// set to synchronise the ADC with the clock
	ADC12_COMMON->CCR |= ADC12_CCR_CKMODE_0;

	// ADEN must be = 0 for configuration (is the default)
	ADC1->CR &= ~ADC_CR_ADVREGEN; // clear voltage regulator enable
	ADC1->CR |= ADC_CR_ADVREGEN_0; // set ADVREGEN TO 01
	ADC1->CR &= ~ADC_CR_ADCALDIF; // clear bit to enable Single-ended-input

	// calibrate the ADC (self calibration routine)
	ADC1->CR |= ADC_CR_ADCAL;
	while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL); // Waiting for the calibration to finish

	// select the channel, only one conversion so it goes in
	//  the first conversion slot (SQ1)
	//  and the L value should be 0 (L=0000 does one conversion only)
	ADC1->SQR1 = 0; // clear any existing channel requests
	ADC1->SQR1 |= 0x03 << ADC_SQR1_SQ1_Pos; // request channel 3 (PA2)
	ADC1->SQR1 |= 0x00 << ADC_SQR1_L_Pos; // set the number of channels to read

	// continuous mode
	ADC1->CFGR |= ADC_CFGR_CONT;

	// Enable ADC
	ADC1->CR |= ADC_CR_ADEN;

	while (ADC1->ISR == 0); //Wait the ADC to be ready.

	// request the process to start
	// only need once in continuous mode
	ADC1->CR |= ADC_CR_ADSTART;

	uint16_t value = 0;
	uint8_t scale = 0;

    /* Loop forever */
	for(;;) {

		// Wait for the end of conversion
		while(!(ADC1->ISR &= ADC_ISR_EOC));

		value = ADC1->DR;

		// read the value from ADC1->DR,
		// full range is 12 bits (0xFFF maximum)
		// divide the scale into 8 even partitions (for 8 leds)
		scale = value / (0xfff / 8);

		if (scale > 7) {
			scale = 7;
		}

		*led_register = 1 << scale ;
	}
}

void SingleReadMultiChannelADC() {

	// get a pointer to the location of the LEDs
	uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;

	// enable the clock for ADC1
	RCC->AHBENR |= RCC_AHBENR_ADC12EN;

	// set to synchronise the ADC with the clock
	ADC12_COMMON->CCR |= ADC12_CCR_CKMODE_0;

	// ADEN must be = 0 for configuration (is the default)
	ADC1->CR &= ~ADC_CR_ADVREGEN; // clear voltage regulator enable
	ADC1->CR |= ADC_CR_ADVREGEN_0; // set ADVREGEN TO 01
	ADC1->CR &= ~ADC_CR_ADCALDIF; // clear bit to enable Single-ended-input

	// calibrate the ADC (self calibration routine)
	ADC1->CR |= ADC_CR_ADCAL;
	while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL); // Waiting for the calibration to finish

/*
 	// can change the sampling time (longer is more accurate)
	ADC1->SMPR1 |= 7 << ADC_SMPR1_SMP0_Pos;
	ADC1->SMPR1 |= 7 << ADC_SMPR1_SMP1_Pos;
	ADC1->SMPR1 |= 7 << ADC_SMPR1_SMP2_Pos;
*/

	// We want to read from two channels each sequence
	//  the first channel goes in SQ1
	//  the second channel goes in SQ2
	//  the number of channels to read = 2, so the L value is 1
	ADC1->SQR1 = 0;
	ADC1->SQR1 |= 0x02 << ADC_SQR1_SQ1_Pos; // set the request for channel 2
	ADC1->SQR1 |= 0x03 << ADC_SQR1_SQ2_Pos; // set the request for channel 3
	ADC1->SQR1 |= 0x01 << ADC_SQR1_L_Pos; // set the number of channels to read

	// single shot mode
	ADC1->CFGR &= ~ADC_CFGR_CONT;

	// Enable the ADC
	ADC1->CR |= ADC_CR_ADEN;

	// Wait the ADC to be ready.
	while (ADC1->ISR == 0);

	uint16_t value_1 = 0;
	uint16_t value_2 = 0;

    /* Loop forever */
	for(;;) {

		// request the process to start
		ADC1->CR |= ADC_CR_ADSTART;

		// Wait for the end of the first conversion
		while(!(ADC1->ISR & ADC_ISR_EOC));

		// read the first value
		value_1 = ADC1->DR;

		while(!(ADC1->ISR & ADC_ISR_EOC));

		// read the second value
		value_2 = ADC1->DR;

		// reset the sequence flag
		ADC1->ISR |= ADC_ISR_EOS;


		// full range is 12 bits (0xFFF maximum)
		// divide the scale into 4 even partitions (for 4 leds)
		uint8_t scale_1 = value_1 / (0xfff / 4);

		if (scale_1 > 3) {
			scale_1 = 3;
		}

		// full range is 12 bits (0xFFF maximum)
		// divide the scale into 4 even partitions (for 4 leds)
		uint8_t scale_2 = value_2 / (0xfff / 4);

		if (scale_2 > 3) {
			scale_2 = 3;
		}

		// draw the state of channel 2 in the first 4 LEDs
		// and the state of channel 3 in the second set of 4 LEDs
		*led_register = 1 << scale_1 | 1 << (scale_2 + 4);
	}
}
