#include <stdint.h>

typedef struct PID {
	int32_t Kp, Ki, Kd, threshold, scale, min, max;
	int32_t	error, totalError, errorRate, output;
} PID;

// Time in milliseconds.
void updatePid(PID *pid, int32_t error);