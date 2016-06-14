#include "pk_robocon_auto.h"
#include "hybrid_pneumatic.h"
#include "hybrid_laser.h"
#include "pk_climb.h"
#include "hybrid_gpio.h"
#include "pk_arm.h"

int auto_state = 0;
int auto_action = 0;

void auto_PREPARATION() {
	pneumatics.P1 = PNEUMATIC_POLE_RELEASE;
	pneumatics.P3 = PNEUMATIC_PROPELLER_GRIP;
}

void auto_MOVE_TO_POLE() {
	enterPole();
}

void auto_GRIP_POLE() {
	pneumatics.P1 = PNEUMATIC_POLE_GRIP;
}


void auto_CLIMB_POLE() {
	if (getLS(LS_CLIMB)) {
		sendClimbCommand(0);
	} else {
		sendClimbCommand(1200);
		auto_action++;
	}
}

void auto_RAISE_ARM() {
	if (getLS(LS_ARM_TOP)) {
		sendArmCommand(0);
	} else {
		sendArmCommand(40);
		auto_action++;
	}
}

void auto_INSTALL_PROPELLER() {
	pneumatics.P2 = PNEUMATIC_SLIDER_IN;
}

void auto_LOWER_ARM() {
	
}

void auto_RESTORE() {
	pneumatics.P2 = PNEUMATIC_SLIDER_OUT;
}

void auto_next() {
	auto_state++;
}

void auto_execute() {
	auto_action = 0;
	if (auto_state == PREPARATION) {
		auto_PREPARATION();
	}
	if (auto_state == MOVE_TO_POLE) {
		auto_MOVE_TO_POLE();
	}
	if (auto_state == GRIP_POLE) {
		auto_GRIP_POLE();
	}
	if (auto_state == CLIMB_POLE) {
		auto_CLIMB_POLE();
	}
	if (auto_state == RAISE_ARM) {
		auto_RAISE_ARM();
	}
	if (auto_state == INSTALL_PROPELLER) {
		auto_INSTALL_PROPELLER();
	}
	if (auto_state == LOWER_ARM) {
		auto_LOWER_ARM();
	}
	if (auto_state == RESTORE) {
		auto_RESTORE();
	}
	if (auto_action == 0) {
		auto_state++;
	}
}
