/*
 * median_filter.c
 *
 *  Created on: May 9, 2025
 *      Author: maxyi
 */
#include <filters.h>

// 2 median filter algorithms are considered
// 1. Insertion sort - sorting the array and selecting the value at the centre, time = O(k), space = O(k)
// 2. Two-heap - max heap for the lower half of values and min heap for the upper half, median always on top of either heap, time = O(logk), space = O(k)
// where k is the length of the sliding window array

// Although two-heap approach is more time efficient,
// the insertion sort method requires less setup due to a more simpler algorithm
// and faster for very small windows (e.g. 5)
// qsort is also considered but the overhead outweigh its time efficiency especially in small arrays
// hence, insertion sort is implemented below


// uint16_t sliding window arr
// uint16_t will work for lidarpwm, servopwm, gyro, accel in PTU, adjust if needed
// uint8_t length n

//use WINDOWLEN = 5
// --------- FILTER INITIALISATION ---------
void initFilters(Filter *filters, uint16_t init_value) {
    for (int i = 0; i < NUM_DATA; i++) {
        for (int j = 0; j < WINDOWLEN; j++) {
            filters[i].window[j] = init_value;
        }
        filters[i].idx = 0;
    }
}


// --------- HELPER FUNCTIONS ---------

// Insert new_value into sliding window buffer of filter, update index cyclically
void slidingWindow(Filter* filter, uint16_t new_value) {
    filter->window[filter->idx] = new_value;
    filter->idx = (filter->idx + 1) % WINDOWLEN;
}

// Insertion sort used for small fixed-size array sorting
void insertionSort(uint16_t* arr, uint8_t n) {
    for (uint8_t i = 1; i < n; i++) {
        uint16_t key = arr[i];
        int8_t j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}


// --------- MEDIAN FILTER ---------

// Apply median filter on new_value using sliding window buffer inside filter
uint16_t getMedian(Filter* filter, uint16_t new_value) {
    slidingWindow(filter, new_value);

    uint16_t temp[WINDOWLEN];
    memcpy(temp, filter->window, sizeof(temp));

    insertionSort(temp, WINDOWLEN);
    return temp[WINDOWLEN / 2];  // middle element
}


// --------- MOVING AVERAGE FILTER ---------

// Apply moving average filter on new_value using sliding window buffer inside filter
uint16_t getMovingAverage(Filter* filter, uint16_t new_value) {
    slidingWindow(filter, new_value);

    uint32_t sum = 0;
    for (uint8_t i = 0; i < WINDOWLEN; i++) {
        sum += filter->window[i];
    }
    return (uint16_t)(sum / WINDOWLEN);
}


// --------- INLINE FILTER FOR SINGLE AXIS ---------

// Inline helper for median filtering of single axis value
static inline int16_t filter_axis(Filter* filter, int16_t value) {
    return (int16_t)getMedian(filter, (uint16_t)value);
}


// --------- SENSOR DATA FILTERING ---------

// Filter gyro data (roll, pitch, yaw) using dedicated filters
void filter_gyro_data(Filter* dataFilters,
                      int16_t roll_rate, int16_t pitch_rate, int16_t yaw_rate,
                      int16_t* filtered_roll, int16_t* filtered_pitch, int16_t* filtered_yaw) {

    *filtered_roll  = filter_axis(&dataFilters[GYRO_X], roll_rate);
    *filtered_pitch = filter_axis(&dataFilters[GYRO_Y], pitch_rate);
    *filtered_yaw   = filter_axis(&dataFilters[GYRO_Z], yaw_rate);
}

// Filter accelerometer data (x, y, z) using dedicated filters
void filter_accel_data(Filter* dataFilters,
                       int16_t acc_x, int16_t acc_y, int16_t acc_z,
                       int16_t* filtered_acc_x, int16_t* filtered_acc_y, int16_t* filtered_acc_z) {

    *filtered_acc_x = filter_axis(&dataFilters[ACCEL_X], acc_x);
    *filtered_acc_y = filter_axis(&dataFilters[ACCEL_Y], acc_y);
    *filtered_acc_z = filter_axis(&dataFilters[ACCEL_Z], acc_z);
}

// Filter all sensor data: lidar (median filter), gyro and accelerometer (median per axis)
void filter_all_sensor_data(Filter* dataFilters,
                            uint16_t last_period,
                            int16_t roll_rate, int16_t pitch_rate, int16_t yaw_rate,
                            int16_t acc_x, int16_t acc_y, int16_t acc_z,
                            uint16_t* filtered_lidar,
                            int16_t* filtered_roll, int16_t* filtered_pitch, int16_t* filtered_yaw,
                            int16_t* filtered_acc_x, int16_t* filtered_acc_y, int16_t* filtered_acc_z) {

    *filtered_lidar = getMedian(&dataFilters[LIDAR], last_period);

    filter_gyro_data(dataFilters, roll_rate, pitch_rate, yaw_rate,
                     filtered_roll, filtered_pitch, filtered_yaw);

    filter_accel_data(dataFilters, acc_x, acc_y, acc_z,
                      filtered_acc_x, filtered_acc_y, filtered_acc_z);
}

