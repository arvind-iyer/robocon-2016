#ifndef _HYBRID_PNEUMATIC
#define _HYBRID_PNEUMATIC

#include <stdbool.h>

typedef struct {
	bool P1;
	bool P2;
	bool P3;
	bool P4;
} Pneumatics;

void hybridPneumaticInit();
void hybridPneumaticControl();
void setPneumaticState(bool pneumatic, bool state);
Pneumatics getPneumaticState();

#endif