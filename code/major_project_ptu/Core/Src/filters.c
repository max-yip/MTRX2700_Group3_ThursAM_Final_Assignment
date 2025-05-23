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


// --------- FILTER INITIALISATION -------
void initFilters(Filter *filters, uint16_t init_value) {
    for (int i = 0; i < NUM_DATA; i++) {
        for (int j = 0; j < WINDOWLEN; j++) {
            filters[i].window[j] = init_value;
        }
        filters[i].idx = 0;
    }
}


// ------ HELPER FUNCTIONS --------
// current index, length of the array, array
void slidingWindow(Filter* filter, uint16_t new_value){
	// insert value in sliding window
	filter->window[filter->idx] = new_value;
	// if current index is length-1, reset current index to 0, else current index ++
	filter->idx = (filter->idx+1) % WINDOWLEN;
}

void insertionSort(uint16_t* arr, uint8_t n){
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


// ------- MEDIAN FILTER ----------
uint16_t getMedian(Filter* filter, uint16_t new_value){
	slidingWindow(filter, new_value);

	//temporary buffer for sorting
	uint16_t temp[WINDOWLEN];
	memcpy(temp, filter->window, sizeof(temp));

	insertionSort(temp, WINDOWLEN);

	return temp[WINDOWLEN/2]; // middle
}


// --------- MOVING AVERAGE FILTER -------
uint16_t getMovingAverage(Filter* filter, uint16_t new_value){
	slidingWindow(filter, new_value);

	uint16_t sum = 0;
	for (uint8_t i = 0; i<WINDOWLEN; i++){
		sum = sum + filter->window[i];
	}

	return sum/WINDOWLEN; // moving avg
}
