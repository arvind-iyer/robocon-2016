#include <stdint.h>

typedef struct PID {
	float Kp, Ki, Kd, threshold, scale, min, max;
	float	error, totalError, errorRate, output;
} PID;

// Time in milliseconds.
void updatePid(PID *pid, float error);