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

void freeArray(Array *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
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
			newBuffer.array[newSize - 1].y = 30;
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

        //Make a line for 2 different points
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
    freeArray(&newBuffer);
    freeArray(&buffer);
	
    return result;
}

int interpolationCircle(int a){
	if (a <= 50){
		a*=2;
		return(100 - Sqrt(10000 - a * a))/2;
	}
	a = a-100;
	a *= 2;
	return(Sqrt(10000 - a * a)+ 100)/2;
}

int32_t * lerp(int32_t *target, int32_t * lastData, uint32_t a){
	int invA = 100 - a;
	static int32_t points[128];
	//initArray (&points,128);
	
    for (int i = 0; i < 128; i++){
		/*insertArray(&points, (Point) {(target.array[i].x * invA) + (lastData.array[i].x * a),
		(target.array[i].y * invA) + (lastData.array[i].y * a)});*/
        points[i] = (target[i] * invA) + (lastData[i] * a);
	}
    return points;
}

int32_t * Interpolation (int32_t* targetData, int32_t* lastData, uint32_t a){
	return lerp (targetData, lastData, interpolationCircle(a));
}


int32_t * calculateAreas(int32_t signal[]) {
	int32_t leftPartition[43];
    int32_t midPartition[42];
	int32_t rightPartition[43];
	int32_t leftAreaSum = 0;
    int32_t midAreaSum = 0;
	int32_t rightAreaSum = 0;
	int lastPossibleIndex = -1;

	//memcpy(leftPartition, signal, 43 * sizeof(int));
    for(int i = 0 ; i < 43 ; i++){
        leftPartition[i] = signal[i];
    }
	//memcpy(midPartition, signal + 43, 42 * sizeof(int));
     for(int i = 0 ; i < 42 ; i++){
        midPartition[i] = signal[i+43];
     }
    //memcpy(rightPartition, signal + 85, 43 * sizeof(int));
     for(int i = 0;i < 43 ; i++){
        rightPartition[i] = signal[i+85];
     }

	for (int i = 0; i < 43; i++) {
		if (leftPartition[i] == -1)
			continue;
		else {
			if (lastPossibleIndex == -1) {
				lastPossibleIndex = i;
				continue;
			}
			int trpHeight = (i + 1) - lastPossibleIndex;
			int32_t area = (leftPartition[i] + leftPartition[lastPossibleIndex])
					/ 2 * trpHeight;
			leftAreaSum += area;
			lastPossibleIndex = i;
		}
	}
	lastPossibleIndex = -1;
	for (int i = 0; i < 42; i++) {
		if (midPartition[i] == -1)
			continue;
		else {
			if (lastPossibleIndex == -1) {
				lastPossibleIndex = i;
				continue;
			}
			int trpHeight = (i + 1) - lastPossibleIndex;
			int32_t area =
					(midPartition[i] + midPartition[lastPossibleIndex]) / 2
							* trpHeight;
			midAreaSum += area;
			lastPossibleIndex = i;
		}
	}
    lastPossibleIndex = -1;
	for (int i = 0; i < 43; i++) {
		if (rightPartition[i] == -1)
			continue;
		else {
			if (lastPossibleIndex == -1) {
				lastPossibleIndex = i;
				continue;
			}
			int trpHeight = (i + 1) - lastPossibleIndex;
			int32_t area =
					(rightPartition[i] + rightPartition[lastPossibleIndex]) / 2
							* trpHeight;
			rightAreaSum += area;
			lastPossibleIndex = i;
		}
	}
    static int32_t result[3];
    result[0] = leftAreaSum/10;
    result[1] = midAreaSum/10;
    result[2] = rightAreaSum/10;
	return result;
}


//Initialize GPIO for CCD LED
void ccdLed_init(){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    GPIO_ResetBits(GPIOA, GPIO_Pin_9);
}

int main() {

	tft_init(2, BLACK, RED, GREEN);
	button_init();
	ticks_init();
    servo_init();
	linear_ccd_init();
	adc_init();
    LED_INIT();
    //ccdLed_init();

	long lastTick = get_ms_ticks();
    
	//Data initialization
	Array points;
	u32 prev_linear_ccd1[128];

    
    //Present Data
    int32_t leftArea;
    int32_t middleArea;
    int32_t rightArea;
    int32_t midToLeftRatio;
    int32_t midToRightRatio;
    
    
    //Prev Data
    int32_t prevLeftArea;
    int32_t prevMiddleArea;
    int32_t prevRightArea;
    int32_t prevMidToLeftRatio;
    int32_t prevMidToRightRatio;
    
    
    int flag = 0; //0 or 1 or 2
    int prevState = 0; // 0 to 15;
    
    int32_t * addressArea;
    
    u32 prev_linear_ccd_buffer1[128];
    
    //Auto State
    s32 autoState = 0;
    
    s32 index = 0;
	
	while (1) {
        //GPIO_SetBits(GPIOA,GPIO_Pin_9);
        if (get_ms_ticks() % 40 == 20 && autoState == 1) {
            
            for (int i = 0; i < 128; i++) {
				Point point = points.array[i];
                tft_put_pixel(i, linear_ccd_buffer1[i], BLACK);
                
			}
            linear_ccd_read();

			
            uint32_t buffer[128];
            
			for (int i = 0; i < 128; i++){
				linear_ccd_buffer1[i] = linear_ccd_buffer1[i] * 100;
		    }
 
            tft_prints(0,0,"Count: %d",get_ms_ticks());
			points = douglasPeucker(linear_ccd_buffer1, 10);
			
			// Do Bresenham Here
			points = bresenham(points, points.used);
            
            
            //Transfer the data into a static array
            for(int i = 0;i<128;i++){
                linear_ccd_buffer1[i] = points.array[i].y;
            }
            
            
            //FREE THE ALLOCATED MEMORY
			freeArray(&points);
            
            
            // Interpolation Here
			if (index > 0) {
				memcpy(linear_ccd_buffer1,Interpolation(linear_ccd_buffer1, prev_linear_ccd_buffer1, 55),128*sizeof(int));
			}
			
			//lastPoints = points;
            for(int i = 0; i < 128; i++){
                prev_linear_ccd_buffer1[i] = linear_ccd_buffer1[i];
            }
            
            //Scale down the value of linear_ccd_buffer1
			for (int i = 0; i < 128; i++) {
                //points.array[i].x /= 100;
                linear_ccd_buffer1[i] /= 100;
			}

			for (int i = 0; i < 128; i++) {
				//linear_ccd_buffer1[i] = points.array[i].y/100;
				tft_put_pixel(i, linear_ccd_buffer1[i], YELLOW);
                //Draw the graph
			}
           
            
            addressArea = calculateAreas(linear_ccd_buffer1);
            leftArea = *(addressArea + 0);
            middleArea  = *(addressArea +1);
            rightArea = *(addressArea + 2);
            
            
            
            midToLeftRatio = (middleArea*1000)/leftArea;
            
            midToRightRatio = (middleArea * 1000)/rightArea;
            
            
            tft_update();
            
            
            if(midToLeftRatio - midToRightRatio <= 500 &&
                midToLeftRatio - midToRightRatio >= -500  ){
                if (prevMidToLeftRatio - prevMidToRightRatio > 500) {
                    //servo_control(3000 - (midToLeftRatio - midToRightRatio)*800/1000); //Turn left
                    servo_control(2000);
                }
                if (prevMidToLeftRatio - prevMidToRightRatio < -500) {
                    //servo_control(3000 - (midToLeftRatio - midToRightRatio)*800/1000); //Turn right
                    servo_control(4000);
                }
                else servo_control(3000);
            }
            if (midToLeftRatio - midToRightRatio > 500) {
                //servo_control(3000 - (midToLeftRatio - midToRightRatio)*800/1000); //Turn left
                servo_control(2000);
            }
            if (midToLeftRatio - midToRightRatio < -500) {
                //servo_control(3000 - (midToLeftRatio - midToRightRatio)*800/1000); //Turn right
                servo_control(4000);
            }
            
            //Control the servo's movement
            if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)){
                servo_control(1800); //Turn right -->left
            }
            
            if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)){ //Lock the servo and stop automation mode
                while(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14));
                autoState = 0;
            }
            
            if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)){
                servo_control(4200); //Turn left --> right
            }
            
            
             //Record the last ccd data
            for(int i = 0 ;i < 128;i++){
                prev_linear_ccd_buffer1[i] = linear_ccd_buffer1[i];
            }
            
            if(prevState == 30){ //update the previous value for every 800ms
                prevLeftArea = leftArea;
                prevMiddleArea = middleArea;
                prevRightArea = rightArea;
                prevMidToLeftRatio = midToLeftRatio;
                prevMidToRightRatio = midToRightRatio;
                prevState = 0;
            }
               
            prevState++; //State for previous values
            
           
            
            //Display all the shit
//            tft_prints(0,1,"Left: %d",leftArea);
//            tft_prints(0,2,"Middle: %d", middleArea);
//            tft_prints(0,3,"Right: %d", rightArea);
//            tft_prints(0,4,"Ratio1: %d", midToLeftRatio);
//            tft_prints(0,5,"Ratio2: %d", midToRightRatio);
//            tft_prints(0,6,"PrevRatio1: %d", prevMidToLeftRatio);
//            tft_prints(0,7,"PrevRatio2: %d", prevMidToRightRatio);
//            tft_prints(0,8,"DiffNow : %d",midToLeftRatio - midToRightRatio);
//            tft_prints(0,9,"DiffPrev: %d",prevMidToLeftRatio - prevMidToRightRatio);
            
              index++;
              if(index > 60000){
                  index = 1;
              }
		}
        
        else if(autoState == 0){ 
            if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)){ //Close lock mode and reactivate automation mode
                while(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14));
                autoState = 1;
            }
            servo_control(3000);
        }
	}

	return 0;
}