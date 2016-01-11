/*
 * main.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: Budi Ryan
 */
#include <stdio.h>
#include <math.h>
#include <stdint.h>

struct Coor
{
    int x;//position
    int y;//velocity
};

//printf("X: %d, Y: %d\n",x0,y0);

void line(int x0, int y0, int x1, int y1) {

  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;//gak usa diganti karena udah sama
  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;

  int err = (dx>dy ? dx : -dy)/2, e2;
  for(;;){
	 printf("X: %d, Y: %d\n",x0,y0);
     if (x0==x1 && y0==y1) break;
     e2 = err;
     if (e2 >-dx) { err -= dy; x0 += sx; }
     if (e2 < dy) { err += dx; y0 += sy; }
   }
}

struct Coor * bresenham(struct Coor buffer[],int size){
	int i;
	int j;
	int indexNum = -1;
	int newSize;
	int newBufferState = 0;
	//printf("haha:%d",buffer[size-1].x);
	if(buffer[0].x == 0 && buffer[size-1].x == 127){
		newSize = size;
		newBufferState = 0;
	}
	if(buffer[size-1].x != 127 && buffer[0].x == 0){
		newSize = size+1;
		newBufferState = 1;
	}
	if(buffer[size-1].x == 127 && buffer[0].x != 0){
		newSize = size+1;
		newBufferState = 2;
	}
	if(buffer[0].x != 0 && buffer[size-1].x != 127){
		newSize = size+2;
		newBufferState = 3;
	}
	printf("newsize: %d\n",newSize);
	printf("newbufferstate=%d\n",newBufferState);
	struct Coor newBuffer[newSize];
	if(newBufferState == 0){
		int c;
			for(c = 0 ; c < newSize ;c++){
				newBuffer[c].x = buffer[c].x;
				newBuffer[c].y = buffer[c].y;
			}
	}
	int h;
	/*for(h = 0;h<newSize;h++){
		printf("data: %d\n",newBuffer[h]);
	}*/
	if(newBufferState == 1){
		int a;
		newBuffer[newSize-1].x = 127;
		newBuffer[newSize-1].y = 127;
		for(a = 0; a < size; a++){
			newBuffer[a].x = buffer[a].x;
			newBuffer[a].y = buffer[a].y;
		}
	}
	/*for(h = 0;h<newSize;h++){
			printf("data: %d\n",newBuffer[h]);
		}*/
	if(newBufferState == 2){
		int b;
			newBuffer[0].x = 0;
			newBuffer[0].y = 0;
			for(b = 1; b< newSize; b++){
				newBuffer[b].x = buffer[b-1].x;
				newBuffer[b].y = buffer[b-1].y;
			}
	}
	/*for(h = 0;h<newSize;h++){
				printf("data: %d\n",newBuffer[h]);
	}*/
	if(newBufferState == 3){
		int d;
			newBuffer[0].x = 0;
			newBuffer[0].y = 0;
			newBuffer[newSize - 1].x = 127;
			newBuffer[newSize - 1].y = 0;
			for(d = 1 ; d < newSize - 1 ; d++){
				newBuffer[d].x  = buffer[d-1].x;
				newBuffer[d].y  = buffer[d-1].y;
			}

	}
	/*for(h = 0;h<newSize;h++){
		printf("data: %d\n",newBuffer[h]);
	}*/


	struct Coor result[128];
	for(i  = 1; i < newSize ; i++){
		int currentX = newBuffer[i].x;
		int currentY = newBuffer[i].y;
		int lastX = newBuffer[i-1].x;
		int lastY = newBuffer[i-1].y;
		//printf("Y: %d",buffer[i-1].y);

		//the bresenham side
		int dx = abs(currentX-lastX), sx = lastX < currentX ? 1 : -1;
		int dy = abs(currentY-lastY), sy = lastY < currentY ? 1 : -1;
		int err = (dx>dy ? dx : -dy)/2, e2;


		for(;;){
		  if(lastX != indexNum){
			  indexNum++;
			  result[indexNum].x = lastX;
			  result[indexNum].y = lastY;
		  }

		  if ((lastX == currentX) && (lastY==currentY)) {
			  result[indexNum].x = lastX;
			  result[indexNum].y = lastY;
			  break;
		  }

		  e2 = err;
		  if (e2 >-dx) { err -= dy; lastX += sx; }
		  if (e2 < dy) { err += dx; lastY += sy; }
		}

	}
	for(j = 0;j<128;j++){
		printf("X: %d Y:%d\n",result[j].x,result[j].y);
	}
	return result;
}




int main(){
	//line(45,50,90,55); DEBUGGInG

	//line(5, 0, 9, 50);

	struct Coor A[6];
		int i,j;
		A[0].x = 0;
		A[0].y = 74;

		A[1].x = 4;
		A[1].y = 90;

		A[2].x = 5;
		A[2].y = 40;

		A[3].x = 7;
		A[3].y = 100;

		A[4].x = 9;
		A[4].y = 135;

		A[5].x = 125;
		A[5].y = 160;

		int size = sizeof(A)/sizeof(struct Coor);
		//printf("SIZE: %d",size);
		bresenham(A,size);


	return 0;
}
