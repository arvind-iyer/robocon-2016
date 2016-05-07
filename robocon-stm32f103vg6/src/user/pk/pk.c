#include "pk.h"

void pk_init() {
	pk_movement_init();
	pk_pid_init();
	brushlessStartup();
	hybridPneumaticInit();
}
