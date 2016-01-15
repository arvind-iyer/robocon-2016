#include "main.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct Point {
	int x;
    float y;
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

float pointDistanceToLine(float x, float y, float x1, float y1, float x2, float y2) {
	float a = x - x1;
	float b = y - y1;
	float c = x2 - x1;
	float d = y2 - y1;
	return abs(a * d - c * b) / Sqrt(c * c + d * d);
}

void douglasPeuckerRecursion(float signal[], float list[], float e,
		int start, int end) {
	int index = -1;
	int count = start + 1;
	float distance = 0.0;
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

Array douglasPeucker(float buff[], float e) {
	float list[128];
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
    
    Array newBuffer;
	initArray(&newBuffer, 128);
    newBuffer.used = bufferSize;


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


	//INITIALIZATION OF THE 4 NEW BUFFER ARRAYS
	if(newBufferState == 0){
		int c;
            for(c = 0 ; c < newSize ;c++){
                newBuffer.array[c].x = buffer.array[c].x;
                newBuffer.array[c].y = buffer.array[c].y;
            }
	}
	if(newBufferState == 1){
		newBuffer.array[newSize-1].x = 127;
		newBuffer.array[newSize-1].y = 127;
        newBuffer.used++;
		for(int a = 0; a < newSize; a++){
			newBuffer.array[a].x = buffer.array[a].x;
			newBuffer.array[a].y = buffer.array[a].y;
		}
	}
	if(newBufferState == 2){
        newBuffer.array[0].x = 0;
        newBuffer.array[0].y = 0;
        newBuffer.used++;
        for(int b = 1; b< newSize; b++){
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
            newBuffer.used += 2;
			for(int d = 1 ; d < newSize - 1 ; d++){
				newBuffer.array[d].x  = buffer.array[d-1].x;
				newBuffer.array[d].y  = buffer.array[d-1].y;
			}

	}

	//INITIALIZE THE RESULT ARRAY
	Array result;
	initArray(&result, 128);
    result.used = newBuffer.used;


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
              result.used++;
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
       
    //FREE THE ALLOCATED MEMORY
    tft_prints(0,5,"%d",result.used);
    freeArray(&newBuffer);
    freeArray(&buffer);
	
    return result;
}


struct Coor{
    int x;
    float y;
};

void quickSort(struct Coor * a, int n) {
	int i, j, p;
    struct Coor t;
	if (n < 2)
		return;
	p = a[n / 2].x;
	for (i = 0, j = n - 1;; i++, j--) {
		while (a[i].x < p)
			i++;
		while (p < a[j].x)
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

float interpolationCircle(float a){
	if (a <= 0.5){
		a*=2;
		return(1 - Sqrt(1 - a * a))/2;
	}
	a = a-1;
	a *= 2;
	return(Sqrt(1 - (a * a))+ 1)/2;
}


struct Coor * lerp(struct Coor * target,struct Coor * lastData, float a){
	float invA = 1 - a;
    static struct Coor points[128];
	
    for (int i = 0; i < 128; i++){
        //X --> (target.array[i].x * invA) + (lastData.array[i].x * a)
        //Y --> (target.array[i].y * invA) + (lastData.array[i].y * a)
        points[i].x = (float)(target[i].x * invA) + (float)(lastData[i].x * a);
        points[i].y = (float)(target[i].y * invA) + (float)(lastData[i].y * a);
	}
    quickSort(points,128);
    return points;
}

struct Coor * Interpolation (struct Coor * targetData, struct Coor * lastData, float a){
	return lerp (targetData, lastData, interpolationCircle(a));
}


float * calculateAreas(float signal[]) {
	float leftPartition[43];
    float midPartition[42];
	float rightPartition[43];
	float leftAreaSum = 0;
    float midAreaSum = 0;
	float rightAreaSum = 0;
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
			float area = (leftPartition[i] + leftPartition[lastPossibleIndex])
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
    static float result[3];
    result[0] = leftAreaSum;
    result[1] = midAreaSum;
    result[2] = rightAreaSum;
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
    float prev_linear_ccd_buffer1[128];
    struct Coor buffers[128]; //For interpolation
    struct Coor prev_buffers[128]; //For interpolation

    
    //Present Data
    float leftArea;
    float middleArea;
    float rightArea;
    float midToLeftRatio;
    float midToRightRatio;
    float diffLeftRight;
    
    
    //Prev Data
    float prevLeftArea;
    float prevMiddleArea;
    float prevRightArea;
    float prevMidToLeftRatio;
    float prevMidToRightRatio;
    float prevDiffLeftRight;
    
    
    int flag = 0; //0 or 1 or 2
    int prevState = 0; // 0 to 15;
    
    float * addressArea;
    
    
    //Auto State
    s32 autoState = 0; //Press button 14 to change between states
    s32 interpolationIndex = 0;//Dont use interpolation for the first loop iteration
	
	while (1) {
        //GPIO_SetBits(GPIOA,GPIO_Pin_9);
        if (get_ms_ticks() % 40 == 20 && autoState == 1) {
            
            for (int i = 0; i < 128; i++) {
				Point point = points.array[i];
                tft_put_pixel(i, linear_ccd_buffer1[i], BLACK);
			}
            
            
            linear_ccd_read();

			
            int32_t buffer[128];
            
 
            //tft_prints(0,0,"Count: %d",get_ms_ticks());
			points = douglasPeucker(linear_ccd_buffer1, 4);
            
            
            
            tft_prints(0,4,"%d",points.used); //<128
            
            
            // Do Bresenham Here
			points = bresenham(points, points.used);
            
            
            //Transfer the data into a static array
            /*for(int i = 0;i < 128;i++){
                linear_ccd_buffer1[i] = points.array[i].y;
            }*/
            //Transfer the data into a static struct
            for(int i = 0; i < 128 ;i++){
                buffers[i].x = i;
                buffers[i].y = points.array[i].y;
            }
            
            
            //FREE THE ALLOCATED MEMORY
			freeArray(&points);
            
            
            // Interpolation Here
			if (interpolationIndex > 0) {
                struct Coor* interpolationResult = Interpolation(buffers,prev_buffers,0.55);
                for(int i = 0 ; i < 128 ;i++){
                    buffers[i] = *(interpolationResult + i);
                }
			}
            
            //Record the last array to the present array
            for(int i = 0; i < 128; i++){
                prev_buffers[i] = buffers[i];
            }
            
            //Restore the data from struct to linear_ccd_buffer1
			for (int i = 0; i < 128; i++) {
                linear_ccd_buffer1[i] = buffers[i].y;
				tft_put_pixel(i, linear_ccd_buffer1[i], YELLOW);
			}
           
            
            addressArea = calculateAreas(linear_ccd_buffer1);
            leftArea = *(addressArea + 0);
            middleArea  = *(addressArea +1);
            rightArea = *(addressArea + 2);
            
            
            
            midToLeftRatio = (middleArea*1000)/leftArea;
            
            midToRightRatio = (middleArea * 1000)/rightArea;
            
            diffLeftRight = midToLeftRatio - midToRightRatio;
           
            
            
            if(diffLeftRight <= 550 && diffLeftRight >= -550  ){
                if (prevDiffLeftRight - prevDiffLeftRight > 550) {
                    //servo_control(3000 - (midToLeftRatio - midToRightRatio)*800/1000); //Turn left
                    servo_control(2000);
                }
                if (diffLeftRight < -550) {
                    //servo_control(3000 - (midToLeftRatio - midToRightRatio)*800/1000); //Turn right
                    servo_control(4000);
                }
                else servo_control(3000);
            }
            if (diffLeftRight > 550) {
                //servo_control(3000 - (midToLeftRatio - midToRightRatio)*800/1000); //Turn left
                servo_control(2000);
            }
            if (diffLeftRight< -550) {
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
            
            if(prevState == 60){ //update the previous value for every 800ms
                prevLeftArea = leftArea;
                prevMiddleArea = middleArea;
                prevRightArea = rightArea;
                prevMidToLeftRatio = midToLeftRatio;
                prevMidToRightRatio = midToRightRatio;
                prevDiffLeftRight = prevMidToLeftRatio - prevMidToRightRatio;
                prevState = 0;
            }
               
            prevState++; //State for previous values
        
            
            //Display all the shit
//            tft_prints(0,0,"Left: %f",leftArea);
//            tft_prints(0,1,"Middle: %f", middleArea);
//            tft_prints(0,2,"Right: %f", rightArea);
//            tft_prints(0,3,"RatioLeft: %f", midToLeftRatio);
//            tft_prints(0,4,"RatioRight: %f", midToRightRatio);
//            tft_prints(0,5,"PrevRatiotLeft: %f", prevMidToLeftRatio);
//            tft_prints(0,6,"PrevRatioRight: %f", prevMidToRightRatio);
//            tft_prints(0,7,"DiffNow : %f",diffLeftRight);
//            tft_prints(0,8,"DiffPrev: %f",prevDiffLeftRight);
//              tft_update();
              interpolationIndex++;
              if(interpolationIndex > 60000){
                  interpolationIndex = 1;
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