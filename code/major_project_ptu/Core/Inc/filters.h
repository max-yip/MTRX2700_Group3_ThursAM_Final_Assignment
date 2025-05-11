/*
 * median_filter.h
 *
 *  Created on: May 9, 2025
 *      Author: maxyi
 */

#ifndef INC_FILTERS_H_
#define INC_FILTERS_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "stm32f303xc.h"

#define WINDOWLEN 11
#define NUM_DATA 7 // number of signals that would require filtering
#define LIDAR    0
#define GYRO_X   1
#define GYRO_Y   2
#define GYRO_Z   3
#define ACCEL_X  4
#define ACCEL_Y  5
#define ACCEL_Z  6


typedef struct{
	uint16_t window[WINDOWLEN];
	uint8_t idx;
}MedianFilter;


void initFilters(MedianFilter *filters, uint16_t init_value);

void slidingWindow(MedianFilter* filter, uint16_t new_value);

void insertionSort(uint16_t* arr, uint8_t n);

uint16_t getMedian(MedianFilter* filter, uint16_t new_value);

uint16_t getMovingAverage(MedianFilter* filter, uint16_t new_value);

#endif /* INC_FILTERS_H_ */
