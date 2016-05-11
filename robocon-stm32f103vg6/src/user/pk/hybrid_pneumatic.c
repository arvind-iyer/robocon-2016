#include "hybrid_pneumatic.h"
#include "pneumatic.h"

Pneumatics pneumatics, prevPneumatics;

/**
  * @brief Initializes the Hybrid's pneumatic variables
   */

void hybridPneumaticInit() {
	pneumatics.P1 = pneumatics.P2 = pneumatics.P3 = pneumatics.P4 = false;
	prevPneumatics.P1 = prevPneumatics.P2 = prevPneumatics.P3 = prevPneumatics.P4 = false;
}

/**
  * @brief Controlling the hybrid's pneumatics (call to update) if it detects difference with previous state, will update
  * @param None.
  * @retval None.
   */

void hybridPneumaticControl() {
	if(pneumatics.P1 != prevPneumatics.P1) {
		prevPneumatics.P1 = pneumatics.P1;
		pneumatic_control(GPIOE, GPIO_Pin_15, pneumatics.P1);
	}
	if(pneumatics.P2 != prevPneumatics.P2) {
		prevPneumatics.P2 = pneumatics.P2;
		pneumatic_control(GPIOE, GPIO_Pin_13, pneumatics.P2);
	}
	if(pneumatics.P3 != prevPneumatics.P3) {
		prevPneumatics.P3 = pneumatics.P3;
		pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
	}
}

/**
  * @brief Getter function for hybrid pneumatic states
  * @param None.
  * @retval pneumatic struct
   */

Pneumatics getPneumaticState() {
	return pneumatics;
}
//  WTF is this
/*void setPneumaticState(bool pneumatic, bool state) {
	pneumatic = state;
}*/
