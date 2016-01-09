#include "main.h"
#include <math.h>
#include <stdio.h>
#include <cstring>

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
	return abs(a * d - c * b) / pow(c * c + d * d, 0.5);
}

void douglasPeuckerRecursion(uint32_t signal[], uint32_t list[], float e,
		int start, int end) {
	int index = -1;
	int count = start + 1;
	float distance = 0;
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
			list[index] = 1;
			douglasPeuckerRecursion(signal, list, e, start, index);
			douglasPeuckerRecursion(signal, list, e, index, end);
		}
	}
}

int douglasPeucker(uint32_t buff[], float e) {
	uint32_t list[128];
	douglasPeuckerRecursion(buff, list, e, 0, 127);
	int count = 0;
	for (int i = 0; i < 128; i++) {
		if (list[i] == 0) {
			buff[i] = -1;
			count++;
		}
	}
	return count;
}
int interpolationCircle(float a){
	if (a <= 0.5f){
		a*=2;
		return(1 - (float)Sqrt(1 - a * a))/2;
	}
	a--;
	a *= 2;
	return((float)Sqrt(1 - a * a)+ 1)/2;
}
int Interpolation (uint32_t start, uint32_t end, float a){

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
	uint32_t lastFilteredData[128];
	

	while (1) {

		if (get_ms_ticks() - lastTick >= 20) {
			for (int i = 0; i < 128; i++) {
				if (buffer[i] == -1)
					continue;
				tft_put_pixel(i, buffer[i], BLACK);
			}

			linear_ccd_read();
			buffer = linear_ccd_buffer1;
			buffer = medianFilter(buffer, 5);
			int thing = douglasPeucker(buffer, 20);
			
			if(sizeof(lastFilteredData) >0){
					for (int i = 0; i<128; i++){
						
					}
			}
			double count = calculateAreas(buffer);

			for (int i = 0; i < 128; i++) {
				if (buffer[i] == -1)
					continue;
				tft_put_pixel(i, buffer[i], GREEN);
			}

			tft_prints(0, 0, "%s", stringBuffer);
			tft_prints(0, 1, "%f", count);
			tft_update();
			
			lastTick = get_ms_ticks();
		}
	}

	return 0;
}