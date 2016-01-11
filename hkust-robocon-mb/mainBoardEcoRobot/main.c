#include "main.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct Point {
	int32_t x, y;
} Point;

typedef struct {
	Point *array;
	size_t used;
	size_t size;
} Array;

void initArray(Array *a, size_t initialSize) {
	a->array = (Point *) malloc(initialSize * sizeof(Point));
	a->used = 0;
	a->size = initialSize;
}

void insertArray(Array *a, Point element) {
	if (a->used == a->size) {
		a->size *= 2;
		a->array = (Point *) realloc(a->array, a->size * sizeof(Point));
	}
	a->array[a->used++] = element;
}

void quickSort(uint32_t *a, int n) {
	int i, j, p, t;
	if (n < 2)
		return;
	p = a[n / 2];
	for (i = 0, j = n - 1;; i++, j--) {
		while (a[i] < p)
			i++;
		while (p < a[j])
			j--;
		if (i >= j)
			break;
		t = a[i];
		a[i] = a[j];
		a[j] = t;
	}
	quickSort(a, i);
	quickSort(a + i, n - i);
}

uint32_t * medianFilter(uint32_t * signal, int windowLen) {
	static uint32_t result[128];
	for (int i = windowLen; i < 128 - (windowLen / 2); i++) {
		uint32_t window[windowLen];
		for (int j = 0; j < windowLen; ++j) {
			window[j] = signal[i - (windowLen / 2) + j];
		}
		quickSort(window, sizeof(window) / sizeof(window[0]));
		result[i - (windowLen / 2)] = window[windowLen / 2];
	}
	return result;
}

uint32_t pointDistanceToLine(int x, int y, int x1, int y1, int x2, int y2) {
	uint32_t a = x - x1;
	uint32_t b = y - y1;
	uint32_t c = x2 - x1;
	uint32_t d = y2 - y1;
	return abs(a * d - c * b) / Sqrt(c * c + d * d);
}

void douglasPeuckerRecursion(uint32_t signal[], uint32_t list[], int e,
		int start, int end) {
	int index = -1;
	int count = start + 1;
	int distance = 0;
	while (count != end) {
		if (pointDistanceToLine(count, signal[count], start, signal[start], end,
				signal[end]) > distance) {
			index = count;
			distance = pointDistanceToLine(count, signal[count], start,
					signal[start], end, signal[end]);
		}
		count++;
	}
	if (index != -1) {
		if (distance > e) {
			list[index] = 100;
			douglasPeuckerRecursion(signal, list, e, start, index);
			douglasPeuckerRecursion(signal, list, e, index, end);
		}
	}
}

Array douglasPeucker(uint32_t buff[], int32_t e) {
	uint32_t list[128];
	douglasPeuckerRecursion(buff, list, e, 0, 127);

	Array points;
	initArray(&points, 128);

	for (int i = 0; i < 128; i++) {
		if (list[i] == 0) {
			insertArray(&points, (Point ) { i, list[i] });
		}
	}
	return points;
}
uint32_t *Iterator(uint32_t array[], uint32_t targetArray[]){
	int indexCount = 0;
	for (uint32_t i = 0; i<128;i++){
		if(array[i] == 0){
			targetArray[indexCount] = 0;
			indexCount +=1;
		}
	}
}
int interpolationCircle(int a){
	if (a <= 50){
		a*=2;
		return(100 - Sqrt(10000 - a * a))/2;
	}
	a= a-100;
	a *= 2;
	return(Sqrt(10000 - a * a)+ 100)/2;
}
int lerp(uint32_t target, uint32_t lastData, uint32_t a){
	int invA = 100 - a;
	uint32_t interpolatedTarget =  (lastData * invA) + (target * a);
	return interpolatedTarget;
}
int Interpolation (uint32_t target, uint32_t lastData, uint32_t a){
	return lerp (target, lastData, a);
}
double calculateAreas(uint32_t signal[]) {
	uint32_t leftPartition[64];
	uint32_t rightPartition[64];
	double leftAreaSum = 0;
	double rightAreaSum = 0;
	int lastPossibleIndex = -1;

	memcpy(leftPartition, signal, 64 * sizeof(int));
	memcpy(rightPartition, signal + 64, 64 * sizeof(int));

	for (int i = 0; i < 63; i++) {
		if (leftPartition[i] == -1)
			continue;
		else {
			if (lastPossibleIndex == -1) {
				lastPossibleIndex = i;
				continue;
			}
			int trpHeight = (i + 1) - lastPossibleIndex;
			double area = (leftPartition[i] + leftPartition[lastPossibleIndex])
					/ 2 * trpHeight;
			leftAreaSum += area;
			lastPossibleIndex = i;
		}
	}

	lastPossibleIndex = -1;
	for (int i = 0; i < 63; i++) {
		if (rightPartition[i] == -1)
			continue;
		else {
			if (lastPossibleIndex == -1) {
				lastPossibleIndex = i;
				continue;
			}
			int trpHeight = (i + 1) - lastPossibleIndex;
			double area =
					(rightPartition[i] + rightPartition[lastPossibleIndex]) / 2
							* trpHeight;
			rightAreaSum += area;
			lastPossibleIndex = i;
		}
	}
	return leftAreaSum / rightAreaSum;
}

char stringBuffer[128];

char *uint8tob( uint8_t value ) {
  static uint8_t base = 2;
  static char buffer[8] = {0};

  int i = 8;
  for( ; i ; --i, value /= base ) {
    buffer[i] = "01"[value % base];
  }

  return &buffer[i+1];
}

int x = 1;

void thing(const uint8_t header) {
	switch (header) {
		case 'a':
			stringBuffer[x] = 'A';
			x++;
			break;
	}
}

int main() {
	stringBuffer[0] = 'T';
	
	uart_init(COM3, 9600);
	uart_interrupt_init(COM3, &thing);
	
	uart_tx_byte(COM3, 'x');
	
	tft_init(0, BLACK, SKY_BLUE, GREEN);
	button_init();
	ticks_init();
	linear_ccd_init();
	adc_init();

	long lastTick = get_ms_ticks();
	uint32_t * buffer;
	uint32_t doubleArray[128][1];
	uint32_t lastFilteredData[128];
	uint32_t dataFinishFilter[128];
	

	while (1) {

		if (get_ms_ticks() - lastTick >= 20) {
			for (int i = 0; i < 128; i++) {
				if (dataFinishFilter[i] == -1)
					continue;
				tft_put_pixel(i, dataFinishFilter[i], BLACK);
			}

			linear_ccd_read();
			buffer = linear_ccd_buffer1;
			for (int i =0;i<128;i++){
				buffer[i] = buffer[i]*100;
			}
			buffer = medianFilter(buffer, 5);
			
			Array points = douglasPeucker(buffer, 10);
			
			/*for (int i = 0; i<128; i++) {
				doubleArray[i][0] = buffer[i];
			}*/
			//int thing = douglasPeucker(buffer, 20);
			
			if (sizeof(lastFilteredData) / sizeof(lastFilteredData[0]) >0){
					for (int i = 0; i<sizeof(lastFilteredData)/sizeof(lastFilteredData[0]); i++){
						dataFinishFilter[i] = Interpolation(buffer[i],lastFilteredData[i], interpolationCircle(55));
						dataFinishFilter[i] = dataFinishFilter[i]/10000;
					}
				}
			for (int i = 0; i<128; i++) {
				lastFilteredData[i] = buffer[i];
			}
			//double count = calculateAreas(buffer);

			for (int i = 0; i < 128; i++) {
				//buffer[i] = buffer[i]/100;
				if (dataFinishFilter[i] == -1)
					continue;
				tft_put_pixel(i, dataFinishFilter[i], GREEN);
			}

			tft_prints(0, 0, "%s", stringBuffer);
			//tft_prints(0, 1, "%f", count);
			tft_update();
			
			lastTick = get_ms_ticks();
		}
	}

	return 0;
}