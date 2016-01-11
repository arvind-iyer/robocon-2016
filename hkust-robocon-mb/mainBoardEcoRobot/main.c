#include "main.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct Point {
	uint32_t x, y;
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

Array douglasPeucker(uint32_t buff[], uint32_t e) {
	uint32_t list[128];
	douglasPeuckerRecursion(buff, list, e, 0, 127);

	Array points;
	initArray(&points, 128);

	for (int i = 0; i < 128; i++) {
		if (list[i] == 0) {
			insertArray(&points, (Point ) { i, buff[i] });
		}
	}
	return points;
}

Array bresenham(Array buffer, int bufferSize){
	int i,j;
	int size = bufferSize; //get the size
	int indexNum = -1;
	int newSize; //the new size
	int newBufferState = 0;


	//4 POSSIBILITIES
	if(buffer.array[0].x == 0 && buffer.array[size-1].x == 127){
		newSize = size;
		newBufferState = 0;
	}
	if(buffer.array[size-1].x != 127 && buffer.array[0].x == 0){
		newSize = size+1;
		newBufferState = 1;
	}
	if(buffer.array[size-1].x == 127 && buffer.array[0].x != 0){
		newSize = size+1;
		newBufferState = 2;
	}
	if(buffer.array[0].x != 0 && buffer.array[size-1].x != 127){
		newSize = size+2;
		newBufferState = 3;
	}

	//struct Coor newBuffer[newSize];
	Array newBuffer;
	initArray(&newBuffer, 128);

	//INITIALIZATION OF THE 4 NEW BUFFER ARRAYS
	if(newBufferState == 0){
		int c;
			for(c = 0 ; c < newSize ;c++){
				newBuffer.array[c].x = buffer.array[c].x;
				newBuffer.array[c].y = buffer.array[c].y;
			}
	}
	if(newBufferState == 1){
		int a;
		newBuffer.array[newSize-1].x = 127;
		newBuffer.array[newSize-1].y = 127;
		for(a = 0; a < size; a++){
			newBuffer.array[a].x = buffer.array[a].x;
			newBuffer.array[a].y = buffer.array[a].y;
		}
	}
	if(newBufferState == 2){
		int b;
			newBuffer.array[0].x = 0;
			newBuffer.array[0].y = 0;
			for(b = 1; b< newSize; b++){
				newBuffer.array[b].x = buffer.array[b-1].x;
				newBuffer.array[b].y = buffer.array[b-1].y;
			}
	}

	if(newBufferState == 3){
		int d;
			newBuffer.array[0].x = 0;
			newBuffer.array[0].y = 0;
			newBuffer.array[newSize - 1].x = 127;
			newBuffer.array[newSize - 1].y = 0;
			for(d = 1 ; d < newSize - 1 ; d++){
				newBuffer.array[d].x  = buffer.array[d-1].x;
				newBuffer.array[d].y  = buffer.array[d-1].y;
			}

	}

	//struct Coor result[128];
	//INITIALIZE THE RESULT ARRAY
	Array result;
	initArray(&result, 128);


	for(i  = 1; i < newSize ; i++){
		int currentX = newBuffer.array[i].x;
		int currentY = newBuffer.array[i].y;
		int lastX = newBuffer.array[i-1].x;
		int lastY = newBuffer.array[i-1].y;

		//the bresenham side
		int dx = abs(currentX-lastX), sx = lastX < currentX ? 1 : -1;
		int dy = abs(currentY-lastY), sy = lastY < currentY ? 1 : -1;
		int err = (dx>dy ? dx : -dy)/2, e2;


		for(;;){
		  if(lastX != indexNum){
			  indexNum++;
			  result.array[indexNum].x = lastX;
			  result.array[indexNum].y = lastY;
		  }

		  if ((lastX == currentX) && (lastY==currentY)) {
			  result.array[indexNum].x = lastX;
			  result.array[indexNum].y = lastY;
			  break;
		  }

		  e2 = err;
		  if (e2 >-dx) { err -= dy; lastX += sx; }
		  if (e2 < dy) { err += dx; lastY += sy; }
		}

	}
	//USE THIS FOR DEBUGGING
	/*for(j = 0;j<128;j++){
		printf("X: %d Y:%d\n",result[j].x,result[j].y);
	}*/
    
    //FREE THE ALLOCATED MEMORY
    free(newBuffer.array);
    free(buffer.array);
	return result;
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
Array lerp(Array target, Array lastData, uint32_t a){
	int invA = 100 - a;
	Array points;
	initArray (&points,128);
	for (int i=0;i<128;i++){
		insertArray(&points, (Point) {(target.array[i].x * invA) + (lastData.array[i].x * a),
		(target.array[i].y * invA) + (lastData.array[i].y * a)});
	}
	return points;
}
Array Interpolation (Array targetData, Array lastData, uint32_t a){
	return lerp (targetData, lastData, interpolationCircle(a));
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
	
	
	tft_init(0, BLACK, SKY_BLUE, GREEN);
	button_init();
	ticks_init();
	linear_ccd_init();
	adc_init();

	int lastTick = get_real_ticks();
	
	Array points;
	Array lastPoints;
	
	while (1) {

		if (get_real_ticks() - lastTick >= 20) {
            linear_ccd_read();
			
            for (int i = 0; i < 128; i++) {
				Point point = points.array[i];
				tft_put_pixel(point.x, point.y, BLACK);
			}

			
			for (int i = 0; i < 128; i++){
				linear_ccd_buffer1[i] = linear_ccd_buffer1[i] * 100;
			}
            tft_prints(0,0,"Y: %d",linear_ccd_buffer1[64]);
            tft_prints(0,1,"Count: %d",lastTick);
			points = douglasPeucker(linear_ccd_buffer1, 10);
			
			// Do Bresenham Here
			points = bresenham(points, points.used);
			
			if (lastPoints.used > 0) {
				points = Interpolation(points, lastPoints, 55);
			}
			
			lastPoints  = points;
			for (int i = 0; i < points.used; i++) {
					points.array[i].x /= 100;
					points.array[i].y /= 100;
			}

			for (int i = 0; i < 128; i++) {
				Point point = points.array[i];
				tft_put_pixel(point.x, point.y/100, GREEN);
			}

			tft_prints(0, 0, "%s", stringBuffer);
            tft_update();
            
            //FREE THE ALLOCATED MEMORY
			free(points.array);
            free(lastPoints.array);
			
            
            lastTick = get_real_ticks();
		}
	}

	return 0;
}