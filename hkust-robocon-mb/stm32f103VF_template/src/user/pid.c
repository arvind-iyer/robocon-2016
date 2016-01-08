// Simple PID Implementation
// Created by: Kenta Iwasaki
// Jan-> 7, 2016

#include "pid.h"
#include "approx_math.h"

void updatePid(PID *pid, int32_t error) {
	pid->errorRate = error - pid->error;
	if ((pid->totalError + error) * pid->Ki / pid->scale < pid->max)
		pid->totalError += error;
	pid->error = error;
	
	if (Abs(pid->error) > pid->threshold)
		pid->output = ((pid->error * pid->Kp) + (pid->totalError * pid->Ki) + (pid->errorRate * pid->Kp)) / pid->scale;
	
	if (pid->output >= pid->max) pid->output = pid->max;
	if (pid->output <= pid->min) pid->output = pid->min;
}