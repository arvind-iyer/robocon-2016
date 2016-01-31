#include "main.h"

int main(void)
{
	button_init();
	while(1){
		button_update();
		
		if (button_pressed()){
		}
	}
}
