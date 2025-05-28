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

// ===================
// === Definitions ===
// ===================

#define WINDOWLEN 11             // Size of the filter window (for median/moving average)
#define NUM_DATA 7               // Total number of sensor signals requiring filtering

// Index identifiers for each sensor signal in the filters array
#define LIDAR    0
#define GYRO_X   1
#define GYRO_Y   2
#define GYRO_Z   3
#define ACCEL_X  4
#define ACCEL_Y  5
#define ACCEL_Z  6

// ===================
// === Data Struct ===
// ===================

// Filter structure: circular buffer for windowed data
typedef struct {
    uint16_t window[WINDOWLEN]; // Raw data window
    uint8_t idx;                // Current insert index (circular)
} Filter;

// ======================
// === Filter Methods ===
// ======================

/**
 * @brief Initialize all filters to a given starting value.
 *
 * @param filters Pointer to array of Filter structs
 * @param init_value Initial value to fill each filter window
 */
void initFilters(Filter* filters, uint16_t init_value);

/**
 * @brief Get median-filtered value for a given sensor input.
 *
 * @param filter Pointer to filter struct
 * @param new_value New raw value to insert
 * @return uint16_t Median value
 */
uint16_t getMedian(Filter* filter, uint16_t new_value);

/**
 * @brief Get moving average-filtered value for a given sensor input.
 *
 * @param filter Pointer to filter struct
 * @param new_value New raw value to insert
 * @return uint16_t Averaged value
 */
uint16_t getMovingAverage(Filter* filter, uint16_t new_value);


/**
 * @brief Filter gyroscope data for roll, pitch, yaw.
 */
void filter_gyro_data(Filter* dataFilters,
                      int16_t roll_rate, int16_t pitch_rate, int16_t yaw_rate,
                      int16_t* filtered_roll, int16_t* filtered_pitch, int16_t* filtered_yaw);

/**
 * @brief Filter accelerometer data for x, y, z axes.
 */
void filter_accel_data(Filter* dataFilters,
                       int16_t acc_x, int16_t acc_y, int16_t acc_z,
                       int16_t* filtered_acc_x, int16_t* filtered_acc_y, int16_t* filtered_acc_z);

/**
 * @brief Filter all sensor data: lidar, gyro, accel.
 */
void filter_all_sensor_data(Filter* dataFilters, uint16_t last_period,
                            int16_t roll_rate, int16_t pitch_rate, int16_t yaw_rate,
                            int16_t acc_x, int16_t acc_y, int16_t acc_z,
                            uint16_t* filtered_lidar,
                            int16_t* filtered_roll, int16_t* filtered_pitch, int16_t* filtered_yaw,
                            int16_t* filtered_acc_x, int16_t* filtered_acc_y, int16_t* filtered_acc_z);


#endif /* INC_FILTERS_H_ */
